// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2004 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

#ident "$XORP: xorp/fea/click_socket.cc,v 1.10 2004/12/03 04:59:28 pavlin Exp $"


#include "fea_module.h"
#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"
#include "libxorp/run_command.hh"

#include "libcomm/comm_api.h"

#include <algorithm>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/param.h>
#ifdef HAVE_SYS_LINKER_H
#include <sys/linker.h>
#endif
#include <sys/mount.h>
#include <errno.h>

#include "click_socket.hh"


const string ClickSocket::PROC_LINUX_MODULES_FILE = "/proc/modules";
const string ClickSocket::LINUX_COMMAND_LOAD_MODULE = "/sbin/insmod";
const string ClickSocket::LINUX_COMMAND_UNLOAD_MODULE = "/sbin/rmmod";
const string ClickSocket::CLICK_FILE_SYSTEM_TYPE = "click";

uint16_t ClickSocket::_instance_cnt = 0;
pid_t ClickSocket::_pid = getpid();

const IPv4 ClickSocket::DEFAULT_USER_CLICK_CONTROL_ADDRESS = IPv4::LOOPBACK();
const TimeVal ClickSocket::USER_CLICK_STARTUP_MAX_WAIT_TIME = TimeVal(1, 0);

//
// Click Sockets communication with Click
//

ClickSocket::ClickSocket(EventLoop& eventloop)
    : _eventloop(eventloop),
      _fd(-1),
      _seqno(0),
      _instance_no(_instance_cnt++),
      _is_enabled(false),
      _is_kernel_click(false),
      _is_user_click(false),
      _kernel_click_install_on_startup(false),
      _user_click_command_execute_on_startup(false),
      _user_click_control_address(DEFAULT_USER_CLICK_CONTROL_ADDRESS),
      _user_click_control_socket_port(DEFAULT_USER_CLICK_CONTROL_SOCKET_PORT),
      _user_click_run_command(NULL)
{

}

ClickSocket::~ClickSocket()
{
    string error_msg;

    if (stop(error_msg) != XORP_OK) {
	XLOG_ERROR("Cannot stop the Click socket: %s", error_msg.c_str());
    }

    XLOG_ASSERT(_ol.empty());
}

int
ClickSocket::start(string& error_msg)
{
    if (is_kernel_click()) {
	if (_fd >= 0)
	    return (XORP_OK);

	//
	// Install kernel Click (if necessary)
	//
	if (_kernel_click_install_on_startup) {
	    string error_msg2;

	    // Load the kernel Click modules
	    if (load_kernel_click_modules(error_msg) != XORP_OK) {
		unload_kernel_click_modules(error_msg2);
		return (XORP_ERROR);
	    }

	    // Mount the Click file system
	    if (mount_click_file_system(error_msg) != XORP_OK) {
		unload_kernel_click_modules(error_msg2);
		return (XORP_ERROR);
	    }
	}

	//
	// Open the Click error file (for reading error messages)
	//
	string click_error_filename;
	click_error_filename = _kernel_click_mount_directory + "/errors";
	_fd = open(click_error_filename.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fd < 0) {
	    error_msg = c_format("Cannot open kernel Click error file %s: %s",
				 click_error_filename.c_str(),
				 strerror(errno));
	    return (XORP_ERROR);
	}
    }

    if (is_user_click()) {
	if (_fd >= 0)
	    return (XORP_OK);

	//
	// Execute the Click command (if necessary)
	//
	if (_user_click_command_execute_on_startup) {
	    // Compose the command and the arguments
	    string command = _user_click_command_file;
	    string arguments = c_format("-f %s -p %u",
					_user_click_startup_config_file.c_str(),
					_user_click_control_socket_port);
	    if (! _user_click_command_extra_arguments.empty())
		arguments += " " + _user_click_command_extra_arguments;

	    if (execute_user_click_command(command, arguments) != XORP_OK) {
		error_msg = c_format("Could not execute the user-level Click");
		return (XORP_ERROR);
	    }
	}

	//
	// Open the socket
	//
	struct in_addr in_addr;
	_user_click_control_address.copy_out(in_addr);
	//
	// TODO: XXX: get rid of this hackish mechanism of waiting
	// pre-defined amount of time until the user-level Click program
	// starts responding.
	//
	TimeVal max_wait_time = USER_CLICK_STARTUP_MAX_WAIT_TIME;
	TimeVal curr_wait_time(0, 100000);	// XXX: 100ms
	TimeVal total_wait_time;
	do {
	    //
	    // XXX: try-and-wait a number of times up to "max_wait_time",
	    // because the user-level Click program may not response
	    // immediately.
	    //
	    TimerList::system_sleep(curr_wait_time);
	    total_wait_time += curr_wait_time;
	    _fd = comm_connect_tcp4(&in_addr,
				    htons(_user_click_control_socket_port),
				    COMM_SOCK_BLOCKING);
	    if (_fd >= 0)
		break;
	    if (total_wait_time < max_wait_time) {
		// XXX: exponentially increase the wait time
		curr_wait_time += curr_wait_time;
		if (total_wait_time + curr_wait_time > max_wait_time)
		    curr_wait_time = max_wait_time - total_wait_time;
		XLOG_WARNING("Could not open user-level Click socket: %s. "
			     "Trying again...",
			     strerror(errno));
		continue;
	    }
	    error_msg = c_format("Could not open user-level Click socket: %s",
				 strerror(errno));
	    terminate_user_click_command();
	    return (XORP_ERROR);
	} while (true);

	//
	// Read the expected banner
	//
	vector<uint8_t> message;
	string error_msg2;
	if (force_read_message(message, error_msg2) != XORP_OK) {
	    error_msg = c_format("Could not read on startup from user-level "
				 "Click socket: %s", error_msg2.c_str());
	    terminate_user_click_command();
	    comm_close(_fd);
	    _fd = -1;
	    return (XORP_ERROR);
	}

	//
	// Check the expected banner.
	// The banner should look like: "Click::ControlSocket/1.1"
	//
	do {
	    string::size_type slash1, slash2, dot1, dot2;
	    string banner = string(reinterpret_cast<const char*>(&message[0]),
				   message.size());
	    string version;
	    int major, minor;

	    // Find the version number and check it.
	    slash1 = banner.find('/');
	    if (slash1 == string::npos) {
		error_msg = c_format("Invalid user-level Click banner: %s",
				     banner.c_str());
		goto error_label;
	    }
	    slash2 = banner.find('/', slash1 + 1);
	    if (slash2 != string::npos)
		version = banner.substr(slash1 + 1, slash2 - slash1 - 1);
	    else
		version = banner.substr(slash1 + 1);

	    dot1 = version.find('.');
	    if (dot1 == string::npos) {
		error_msg = c_format("Invalid user-level Click version: %s",
				     version.c_str());
		goto error_label;
	    }
	    dot2 = version.find('.', dot1 + 1);
	    major = atoi(version.substr(0, dot1).c_str());
	    if (dot2 != string::npos)
		minor = atoi(version.substr(dot1 + 1, dot2 - dot1 - 1).c_str());
	    else
		minor = atoi(version.substr(dot1 + 1).c_str());
	    if ((major < CLICK_MAJOR_VERSION)
		|| ((major == CLICK_MAJOR_VERSION)
		    && (minor < CLICK_MINOR_VERSION))) {
		error_msg = c_format("Invalid user-level Click version: "
				     "expected at least %d.%d "
				     "(found %s)",
			   CLICK_MAJOR_VERSION, CLICK_MINOR_VERSION,
			   version.c_str());
		goto error_label;
	    }
	    break;

	error_label:
	    terminate_user_click_command();
	    comm_close(_fd);
	    _fd = -1;
	    return (XORP_ERROR);
	} while (false);

	//
	// Add the socket to the event loop
	//
	if (_eventloop.add_selector(_fd, SEL_RD,
				    callback(this, &ClickSocket::select_hook))
	    == false) {
	    error_msg = c_format("Failed to add user-level Click socket "
				 "to EventLoop");
	    terminate_user_click_command();
	    comm_close(_fd);
	    _fd = -1;
	    return (XORP_ERROR);
	}
    }

    return (XORP_OK);
}

int
ClickSocket::stop(string& error_msg)
{
    if (is_kernel_click()) {
	//
	// Close the Click error file (for reading error messages)
	//
	if (_fd >= 0) {
	    close(_fd);
	    _fd = -1;
	}
	if (unmount_click_file_system(error_msg) != XORP_OK) {
	    string error_msg2;
	    unload_kernel_click_modules(error_msg2);
	    return (XORP_ERROR);
	}
	if (unload_kernel_click_modules(error_msg) != XORP_OK) {
	    return (XORP_ERROR);
	}
    }

    if (is_user_click()) {
	terminate_user_click_command();
	if (_fd >= 0) {
	    //
	    // Remove the socket from the event loop and close it
	    //
	    _eventloop.remove_selector(_fd, SEL_ALL);
	    comm_close(_fd);
	    _fd = -1;
	}
    }

    return (XORP_OK);

    // TODO: check for errors
    UNUSED(error_msg);
}

int
ClickSocket::load_kernel_click_modules(string& error_msg)
{
    list<string>::iterator iter;

    for (iter = _kernel_click_modules.begin();
	 iter != _kernel_click_modules.end();
	 ++iter) {
	const string& module_filename = *iter;
	if (load_kernel_module(module_filename, error_msg) != XORP_OK)
	    return (XORP_ERROR);
    }

    return (XORP_OK);
}

int
ClickSocket::unload_kernel_click_modules(string& error_msg)
{
    list<string>::reverse_iterator riter;

    for (riter = _kernel_click_modules.rbegin();
	 riter != _kernel_click_modules.rend();
	 ++riter) {
	const string& module_filename = *riter;
	if (unload_kernel_module(module_filename, error_msg) != XORP_OK)
	    return (XORP_ERROR);
    }

    return (XORP_OK);
}

int
ClickSocket::load_kernel_module(const string& module_filename,
				string& error_msg)
{
    if (module_filename.empty()) {
	error_msg = c_format("Kernel module filename to load is empty");
	return (XORP_ERROR);
    }

    if (find(_loaded_kernel_click_modules.begin(),
	     _loaded_kernel_click_modules.end(),
	     module_filename)
	!= _loaded_kernel_click_modules.end()) {
	return (XORP_OK);	// Module already loaded
    }

    string module_name = kernel_module_filename2modulename(module_filename);
    if (module_name.empty()) {
	error_msg = c_format("Invalid kernel module filename: %s",
			     module_filename.c_str());
	return (XORP_ERROR);
    }

    //
    // Load the kernel module using system-specific mechanism
    //

#ifdef HOST_OS_FREEBSD
    //
    // Test if the kernel module was installed already
    //
    if (kldfind(module_name.c_str()) >= 0) {
	return (XORP_OK);	// Module with the same name already loaded
    }

    //
    // Load the kernel module
    //
    if (kldload(module_filename.c_str()) < 0) {
	error_msg = c_format("Cannot load kernel module %s: %s",
			     module_filename.c_str(), strerror(errno));
	return (XORP_ERROR);
    }

    _loaded_kernel_click_modules.push_back(module_filename);

    return (XORP_OK);

#endif // HOST_OS_FREEBSD

#ifdef HOST_OS_LINUX
    //
    // Test if the kernel module was installed already
    //
    char buf[1024];
    char name[1024];

    FILE* fh = fopen(PROC_LINUX_MODULES_FILE.c_str(), "r");
    if (fh == NULL) {
	error_msg = c_format("Cannot open file %s for reading: %s",
			     PROC_LINUX_MODULES_FILE.c_str(), strerror(errno));
	return (XORP_ERROR);
    }
    while (fgets(buf, sizeof(buf), fh) != NULL) {
	char* n = name;
	char* p = buf;
	char* s = NULL;

	// Get the module name: the first word in the line
	do {
	    while (xorp_isspace(*p))
		p++;
	    if (*p == '\0') {
		s = NULL;
		break;
	    }

	    while (*p) {
		if (xorp_isspace(*p))
		    break;
		*n++ = *p++;
	    }
	    *n++ = '\0';
	    s = p;
	    break;
	} while (true);

	if (s == NULL) {
	    XLOG_ERROR("%s: cannot get module name for line %s",
		       PROC_LINUX_MODULES_FILE.c_str(), buf);
	    continue;
	}
	if (module_name == string(name)) {
	    fclose(fh);
	    return (XORP_OK);	// Module with the same name already loaded
	}
    }
    fclose(fh);

    //
    // Load the kernel module
    //
    // XXX: unfortunately, Linux doesn't have a consistent system API
    // for loading kernel modules, so we have to relay on user-land command
    // to do this. Sigh...
    //
    string command_line = LINUX_COMMAND_LOAD_MODULE + " " + module_filename;
    int ret_value = system(command_line.c_str());
    if (ret_value != 0) {
	if (ret_value < 0) {
	    error_msg = c_format("Cannot execute system command '%s': %s",
				 command_line.c_str(), strerror(errno));
	} else {
	    error_msg = c_format("Executing system command '%s' "
				 "returned value '%d'",
				 command_line.c_str(), ret_value);
	}
	return (XORP_ERROR);
    }

    _loaded_kernel_click_modules.push_back(module_filename);

    return (XORP_OK);

#endif // HOST_OS_LINUX

#ifdef HOST_OS_MACOSX
    // TODO: implement it
    error_msg = c_format("No mechanism to load a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_MACOSX

#ifdef HOST_OS_NETBSD
    // TODO: implement it
    error_msg = c_format("No mechanism to load a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_NETBSD

#ifdef HOST_OS_OPENBSD
    // TODO: implement it
    error_msg = c_format("No mechanism to load a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_OPENBSD

#ifdef HOST_OS_SOLARIS
    // TODO: implement it
    error_msg = c_format("No mechanism to load a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_SOLARIS

    error_msg = c_format("No mechanism to load a kernel module");
    return (XORP_ERROR);
}

int
ClickSocket::unload_kernel_module(const string& module_filename,
				  string& error_msg)
{
    if (module_filename.empty()) {
	error_msg = c_format("Kernel module filename to unload is empty");
	return (XORP_ERROR);
    }

    if (find(_loaded_kernel_click_modules.begin(),
	     _loaded_kernel_click_modules.end(),
	     module_filename)
	== _loaded_kernel_click_modules.end()) {
	return (XORP_OK);	// Module not loaded
    }

    string module_name = kernel_module_filename2modulename(module_filename);
    if (module_name.empty()) {
	error_msg = c_format("Invalid kernel module filename: %s",
			     module_filename.c_str());
	return (XORP_ERROR);
    }

    //
    // Unload the kernel module using system-specific mechanism
    //

#ifdef HOST_OS_FREEBSD
    //
    // Find the kernel module ID.
    //
    int module_id = kldfind(module_name.c_str());
    if (module_id < 0) {
	error_msg = c_format("Cannot unload kernel module %s: "
			     "module ID not found: %s",
			     module_filename.c_str(), strerror(errno));
	return (XORP_ERROR);
    }

    //
    // Unload the kernel module
    //
    if (kldunload(module_id) < 0) {
	error_msg = c_format("Cannot unload kernel module %s: %s",
			     module_filename.c_str(), strerror(errno));
	return (XORP_ERROR);
    }

    // Remove the module filename from the list of loaded modules
    list<string>::iterator iter;
    iter = find(_loaded_kernel_click_modules.begin(),
		_loaded_kernel_click_modules.end(),
		module_filename);
    XLOG_ASSERT(iter != _loaded_kernel_click_modules.end());
    _loaded_kernel_click_modules.erase(iter);

    return (XORP_OK);

#endif // HOST_OS_FREEBSD

#ifdef HOST_OS_LINUX
    //
    // Unload the kernel module
    //
    // XXX: unfortunately, Linux doesn't have a consistent system API
    // for loading kernel modules, so we have to relay on user-land command
    // to do this. Sigh...
    //
    string command_line = LINUX_COMMAND_UNLOAD_MODULE + " " + module_name;
    int ret_value = system(command_line.c_str());
    if (ret_value != 0) {
	if (ret_value < 0) {
	    error_msg = c_format("Cannot execute system command '%s': %s",
				 command_line.c_str(), strerror(errno));
	} else {
	    error_msg = c_format("Executing system command '%s' "
				 "returned value '%d'",
				 command_line.c_str(), ret_value);
	}
	return (XORP_ERROR);
    }

    // Remove the module filename from the list of loaded modules
    list<string>::iterator iter;
    iter = find(_loaded_kernel_click_modules.begin(),
		_loaded_kernel_click_modules.end(),
		module_filename);
    XLOG_ASSERT(iter != _loaded_kernel_click_modules.end());
    _loaded_kernel_click_modules.erase(iter);

    return (XORP_OK);

#endif // HOST_OS_LINUX

#ifdef HOST_OS_MACOSX
    // TODO: implement it
    error_msg = c_format("No mechanism to unload a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_MACOSX

#ifdef HOST_OS_NETBSD
    // TODO: implement it
    error_msg = c_format("No mechanism to unload a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_NETBSD

#ifdef HOST_OS_OPENBSD
    // TODO: implement it
    error_msg = c_format("No mechanism to unload a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_OPENBSD

#ifdef HOST_OS_SOLARIS
    // TODO: implement it
    error_msg = c_format("No mechanism to unload a kernel module");
    return (XORP_ERROR);
#endif // HOST_OS_SOLARIS

    error_msg = c_format("No mechanism to unload a kernel module");
    return (XORP_ERROR);
}

string
ClickSocket::kernel_module_filename2modulename(const string& module_filename)
{
    string filename, module_name;
    string::size_type slash, dot;
    list<string> suffix_list;

    // Find the file name after the last '/'
    slash = module_filename.rfind('/');
    if (slash == string::npos)
	filename = module_filename;
    else
	filename = module_filename.substr(slash + 1);

    //
    // Find the module name by excluding the suffix after the last '.'
    // if that is a well-known suffix (e.g., ".o" or ".ko").
    //
    suffix_list.push_back(".o");
    suffix_list.push_back(".ko");
    module_name = filename;
    list<string>::iterator iter;
    for (iter = suffix_list.begin(); iter != suffix_list.end(); ++iter) {
	string suffix = *iter;
	dot = filename.rfind(suffix);
	if (dot != string::npos) {
	    if (filename.substr(dot) == suffix) {
		module_name = filename.substr(0, dot);
		break;
	    }
	}
    }

    return (module_name);
}

int
ClickSocket::mount_click_file_system(string& error_msg)
{
    if (_kernel_click_mount_directory.empty()) {
	error_msg = c_format("Kernel Click mount directory is empty");
	return (XORP_ERROR);
    }

    if (! _mounted_kernel_click_mount_directory.empty()) {
	if (_kernel_click_mount_directory
	    == _mounted_kernel_click_mount_directory) {
	    return (XORP_OK);	// Directory already mounted
	}

	error_msg = c_format("Cannot mount Click file system on directory %s: "
			     "Click file system already mounted on "
			     "directory %s",
			     _kernel_click_mount_directory.c_str(),
			     _mounted_kernel_click_mount_directory.c_str());
	return (XORP_ERROR);
    }

    //
    // Test if the Click file system has been installed already.
    //
    // We do this by tesing whether we can access a number of Click files
    // within the Click file system.
    //
    list<string> click_files;
    list<string>::iterator iter;
    size_t files_found = 0;

    click_files.push_back("/config");
    click_files.push_back("/flatconfig");
    click_files.push_back("/packages");
    click_files.push_back("/version");

    for (iter = click_files.begin(); iter != click_files.end(); ++iter) {
	string click_filename = _kernel_click_mount_directory + *iter;
	if (access(click_filename.c_str(), R_OK) == 0) {
	    files_found++;
	}
    }
    if (files_found > 0) {
	if (files_found == click_files.size()) {
	    return (XORP_OK);	// Directory already mounted
	}
	error_msg = c_format("Click file system mount directory contains "
			     "some Click files");
	return (XORP_ERROR);
    }

    //
    // XXX: Linux has different mount(2) API, hence we need to take
    // care of this. Sigh...
    //
    int ret_value = -1;
#ifndef HOST_OS_LINUX
    ret_value = mount(CLICK_FILE_SYSTEM_TYPE.c_str(),
		      _kernel_click_mount_directory.c_str(), 0, 0);
#else // HOST_OS_LINUX
    ret_value = mount("none", _kernel_click_mount_directory.c_str(),
		      CLICK_FILE_SYSTEM_TYPE.c_str(), 0, 0);
#endif // HOST_OS_LINUX

    if (ret_value != 0) {
	error_msg = c_format("Cannot mount Click file system "
			     "on directory %s: %s",
			     _kernel_click_mount_directory.c_str(),
			     strerror(errno));
	return (XORP_ERROR);
    }

    _mounted_kernel_click_mount_directory = _kernel_click_mount_directory;

    return (XORP_OK);
}

int
ClickSocket::unmount_click_file_system(string& error_msg)
{
    if (_mounted_kernel_click_mount_directory.empty())
	return (XORP_OK);	// Directory not mounted

    //
    // XXX: Linux doesn't have unmount(2). Instead, it has umount(2), hence
    // we need to take care of this. Sigh...
    //
    int ret_value = -1;
#ifndef HOST_OS_LINUX
    ret_value = unmount(_mounted_kernel_click_mount_directory.c_str(), 0);
#else // HOST_OS_LINUX
    ret_value = umount(_mounted_kernel_click_mount_directory.c_str());
#endif // HOST_OS_LINUX

    if (ret_value != 0) {
	error_msg = c_format("Cannot unmount Click file system "
			     "from directory %s: %s",
			     _mounted_kernel_click_mount_directory.c_str(),
			     strerror(errno));
	return (XORP_ERROR);
    }

    _mounted_kernel_click_mount_directory.erase();

    return (XORP_OK);
}

int
ClickSocket::execute_user_click_command(const string& command,
					const string& arguments)
{
    if (_user_click_run_command != NULL)
	return (XORP_ERROR);	// XXX: command is already running

    _user_click_run_command = new RunCommand(_eventloop, command, arguments,
					     callback(this, &ClickSocket::user_click_command_stdout_cb),
					     callback(this, &ClickSocket::user_click_command_stderr_cb),
					     callback(this, &ClickSocket::user_click_command_done_cb));
    if (_user_click_run_command->execute() != XORP_OK) {
	delete _user_click_run_command;
	_user_click_run_command = NULL;
	return (XORP_ERROR);
    }

    return (XORP_OK);
}

void
ClickSocket::terminate_user_click_command()
{
    if (_user_click_run_command != NULL) {
	delete _user_click_run_command;
	_user_click_run_command = NULL;
    }
}

void
ClickSocket::user_click_command_stdout_cb(RunCommand* run_command,
					  const string& output)
{
    XLOG_ASSERT(_user_click_run_command == run_command);
    XLOG_INFO("User-level Click stdout output: %s", output.c_str());
}

void
ClickSocket::user_click_command_stderr_cb(RunCommand* run_command,
					  const string& output)
{
    XLOG_ASSERT(_user_click_run_command == run_command);
    XLOG_ERROR("User-level Click stderr output: %s", output.c_str());
}

void
ClickSocket::user_click_command_done_cb(RunCommand* run_command, bool success,
					const string& error_msg)
{
    XLOG_ASSERT(_user_click_run_command == run_command);
    if (! success) {
	XLOG_ERROR("User-level Click command (%s) failed: %s",
		   run_command->command().c_str(), error_msg.c_str());
    }
    delete _user_click_run_command;
    _user_click_run_command = NULL;
}

int
ClickSocket::write_config(const string& element, const string& handler,
			  const string& data, string& errmsg)
{
    if (is_kernel_click()) {
	//
	// Prepare the output handler name
	//
	string output_handler = element;
	if (! output_handler.empty())
	    output_handler += "/" + handler;
	else
	    output_handler = handler;
	output_handler = _kernel_click_mount_directory + "/" + output_handler;

	//
	// Prepare the socket to write the data
	//
	int fd = ::open(output_handler.c_str(), O_WRONLY | O_TRUNC | O_FSYNC);
	if (fd < 0) {
	    errmsg = c_format("Cannot open kernel Click handler '%s' "
			      "for writing: %s",
			      output_handler.c_str(), strerror(errno));
	    return (XORP_ERROR);
	}

	//
	// Write the data
	//
	if (::write(fd, data.c_str(), data.size())
	    != static_cast<ssize_t>(data.size())) {
	    errmsg = c_format("Error writing to kernel Click handler '%s': %s",
			      output_handler.c_str(), strerror(errno));
	    return (XORP_ERROR);
	}
	// XXX: we must close the socket before checking the result
	close(fd);

	//
	// Check the command status
	//
	char error_buf[8 * 1024];
	int error_bytes;
	error_bytes = ::read(_fd, error_buf, sizeof(error_buf));
	if (error_bytes < 0) {
	    errmsg = c_format("Error verifying the command status after "
			      "writing to kernel Click handler: %s",
			      strerror(errno));
	    return (XORP_ERROR);
	}
	if (error_bytes > 0) {
	    errmsg = c_format("Kernel Click command error: %s", error_buf);
	    return (XORP_ERROR);
	}
    }

    if (is_user_click()) {
	//
	// Prepare the output handler name
	//
	string output_handler = element;
	if (! output_handler.empty())
	    output_handler += "." + handler;
	else
	    output_handler = handler;

	//
	// Prepare the data to write
	//
	string config = c_format("WRITEDATA %s %u\n",
				 output_handler.c_str(),
				 static_cast<uint32_t>(data.size()));
	config += data;

	//
	// Write the data
	//
	if (ClickSocket::write(config.c_str(), config.size())
	    != static_cast<ssize_t>(config.size())) {
	    errmsg = c_format("Error writing to user-level Click socket: %s",
			      strerror(errno));
	    return (XORP_ERROR);
	}

	//
	// Check the command status
	//
	bool is_warning, is_error;
	string command_warning, command_error;
	if (check_user_command_status(is_warning, command_warning,
				      is_error, command_error,
				      errmsg)
	    != XORP_OK) {
	    errmsg = c_format("Error verifying the command status after "
			      "writing to user-level Click socket: %s",
			      errmsg.c_str());
	    return (XORP_ERROR);
	}

	if (is_warning) {
	    XLOG_WARNING("User-level Click command warning: %s",
			 command_warning.c_str());
	}
	if (is_error) {
	    errmsg = c_format("User-level Click command error: %s",
			      command_error.c_str());
	    return (XORP_ERROR);
	}
    }

    return (XORP_OK);
}

ssize_t
ClickSocket::write(const void* data, size_t nbytes)
{
    _seqno++;
    return ::write(_fd, data, nbytes);
}

int
ClickSocket::check_user_command_status(bool& is_warning,
				       string& command_warning,
				       bool& is_error,
				       string& command_error,
				       string& errmsg)
{
    vector<uint8_t> buffer;

    is_warning = false;
    is_error = false;

    if (force_read_message(buffer, errmsg) != XORP_OK)
	return (XORP_ERROR);

    //
    // Split the message into lines
    //
    string buffer_str = string(reinterpret_cast<char *>(&buffer[0]));
    list<string> lines;
    do {
	string::size_type idx = buffer_str.find("\n");
	if (idx == string::npos) {
	    if (! buffer_str.empty())
		lines.push_back(buffer_str);
	    break;
	}

	string line = buffer_str.substr(0, idx + 1);
	lines.push_back(line);
	buffer_str = buffer_str.substr(idx + 1);
    } while (true);

    //
    // Parse the message line-by-line
    //
    list<string>::const_iterator iter;
    bool is_last_command_response = false;
    for (iter = lines.begin(); iter != lines.end(); ++iter) {
	const string& line = *iter;
	if (is_last_command_response)
	    break;
	if (line.size() < CLICK_COMMAND_RESPONSE_MIN_SIZE) {
	    errmsg = c_format("User-level Click command line response is too "
			      "short (expected min size %u received %u): %s",
			      static_cast<uint32_t>(CLICK_COMMAND_RESPONSE_MIN_SIZE),
			      static_cast<uint32_t>(line.size()),
			      line.c_str());
	    return (XORP_ERROR);
	}

	//
	// Get the error code
	//
	char separator = line[CLICK_COMMAND_RESPONSE_CODE_SEPARATOR_INDEX];
	if ((separator != ' ') && (separator != '-')) {
	    errmsg = c_format("Invalid user-level Click command line response "
			      "(missing code separator): %s",
			      line.c_str());
	    return (XORP_ERROR);
	}
	int error_code = atoi(line.substr(0, CLICK_COMMAND_RESPONSE_CODE_SEPARATOR_INDEX).c_str());

	if (separator == ' ')
	    is_last_command_response = true;

	//
	// Test the error code
	//
	if (error_code == CLICK_COMMAND_CODE_OK)
	    continue;

	if ((error_code >= CLICK_COMMAND_CODE_WARNING_MIN)
	    && (error_code <= CLICK_COMMAND_CODE_WARNING_MAX)) {
	    is_warning = true;
	    command_warning += line;
	    continue;
	}

	if ((error_code >= CLICK_COMMAND_CODE_ERROR_MIN)
	    && (error_code <= CLICK_COMMAND_CODE_ERROR_MAX)) {
	    is_error = true;
	    command_error += line;
	    continue;
	}

	// Unknown error code
	errmsg = c_format("Unknown user-level Click error code: %s",
			  line.c_str());
	return (XORP_ERROR);
    }

    return (XORP_OK);
}

int
ClickSocket::force_read(string& errmsg)
{
    vector<uint8_t> message;

    if (force_read_message(message, errmsg) != XORP_OK)
	return (XORP_ERROR);

    //
    // Notify observers
    //
    for (ObserverList::iterator i = _ol.begin(); i != _ol.end(); i++) {
	(*i)->clsock_data(&message[0], message.size());
    }

    return (XORP_OK);
}

int
ClickSocket::force_read_message(vector<uint8_t>& message, string& errmsg)
{
    vector<uint8_t> buffer(CLSOCK_BYTES);

    for ( ; ; ) {
	ssize_t got;
	// Find how much data is queued in the first message
	do {
	    got = recv(_fd, &buffer[0], buffer.size(),
		       MSG_DONTWAIT | MSG_PEEK);
	    if ((got < 0) && (errno == EINTR))
		continue;	// XXX: the receive was interrupted by a signal
	    if ((got < 0) || (got < (ssize_t)buffer.size()))
		break;		// The buffer is big enough
	    buffer.resize(buffer.size() + CLSOCK_BYTES);
	} while (true);

	got = read(_fd, &buffer[0], buffer.size());
	if (got < 0) {
	    if (errno == EINTR)
		continue;
	    errmsg = c_format("Click socket read error: %s", strerror(errno));
	    return (XORP_ERROR);
	}
	message.resize(got);
	memcpy(&message[0], &buffer[0], got);
	break;
    }

    return (XORP_OK);
}

void
ClickSocket::select_hook(int fd, SelectorMask m)
{
    string errmsg;

    XLOG_ASSERT(fd == _fd);
    XLOG_ASSERT(m == SEL_RD);
    if (force_read(errmsg) != XORP_OK) {
	XLOG_ERROR("Error force_read() from Click socket: %s", errmsg.c_str());
    }
}

//
// Observe Click sockets activity
//

struct ClickSocketPlumber {
    typedef ClickSocket::ObserverList ObserverList;

    static void
    plumb(ClickSocket& r, ClickSocketObserver* o)
    {
	ObserverList& ol = r._ol;
	ObserverList::iterator i = find(ol.begin(), ol.end(), o);
	debug_msg("Plumbing ClickSocketObserver %p to ClickSocket%p\n",
		  o, &r);
	XLOG_ASSERT(i == ol.end());
	ol.push_back(o);
    }
    static void
    unplumb(ClickSocket& r, ClickSocketObserver* o)
    {
	ObserverList& ol = r._ol;
	debug_msg("Unplumbing ClickSocketObserver%p from "
		  "ClickSocket %p\n", o, &r);
	ObserverList::iterator i = find(ol.begin(), ol.end(), o);
	XLOG_ASSERT(i != ol.end());
	ol.erase(i);
    }
};

ClickSocketObserver::ClickSocketObserver(ClickSocket& cs)
    : _cs(cs)
{
    ClickSocketPlumber::plumb(cs, this);
}

ClickSocketObserver::~ClickSocketObserver()
{
    ClickSocketPlumber::unplumb(_cs, this);
}

ClickSocket&
ClickSocketObserver::click_socket()
{
    return _cs;
}

ClickSocketReader::ClickSocketReader(ClickSocket& cs)
    : ClickSocketObserver(cs),
      _cs(cs),
      _cache_valid(false),
      _cache_seqno(0)
{

}

ClickSocketReader::~ClickSocketReader()
{

}

/**
 * Force the reader to receive data from the specified Click socket.
 *
 * @param cs the Click socket to receive the data from.
 * @param seqno the sequence number of the data to receive.
 * @param errmsg the error message (if an error).
 * @return XORP_OK on success, otherwise XORP_ERROR.
 */
int
ClickSocketReader::receive_data(ClickSocket& cs, uint32_t seqno,
				string& errmsg)
{
    _cache_seqno = seqno;
    _cache_valid = false;
    while (_cache_valid == false) {
	if (cs.force_read(errmsg) != XORP_OK)
	    return (XORP_ERROR);
    }

    return (XORP_OK);
}

/**
 * Receive data from the Click socket.
 *
 * Note that this method is called asynchronously when the Click socket
 * has data to receive, therefore it should never be called directly by
 * anything else except the Click socket facility itself.
 *
 * @param data the buffer with the received data.
 * @param nbytes the number of bytes in the @param data buffer.
 */
void
ClickSocketReader::clsock_data(const uint8_t* data, size_t nbytes)
{
    //
    // Copy data that has been requested to be cached
    //
    _cache_data = string(reinterpret_cast<const char*>(data), nbytes);
    // memcpy(&_cache_data[0], data, nbytes);
    _cache_valid = true;
}
