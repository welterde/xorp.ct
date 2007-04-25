// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2007 International Computer Science Institute
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

#ident "$XORP: xorp/fea/ifconfig_observer_rtsock.cc,v 1.17 2007/04/18 06:20:57 pavlin Exp $"

#include "fea_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"

#include "ifconfig.hh"
#include "ifconfig_observer.hh"


//
// Observe information change about network interface configuration from
// the underlying system.
//
// The mechanism to observe the information is routing sockets.
//


IfConfigObserverRtsock::IfConfigObserverRtsock(IfConfig& ifc)
    : IfConfigObserver(ifc),
      RoutingSocket(ifc.eventloop()),
      RoutingSocketObserver(*(RoutingSocket *)this)
{
#ifdef HAVE_ROUTING_SOCKETS
    register_ifc_primary();
#endif
}

IfConfigObserverRtsock::~IfConfigObserverRtsock()
{
    string error_msg;

    if (stop(error_msg) != XORP_OK) {
	XLOG_ERROR("Cannot stop the routing sockets mechanism to observe "
		   "information about network interfaces from the underlying "
		   "system: %s",
		   error_msg.c_str());
    }
}

int
IfConfigObserverRtsock::start(string& error_msg)
{
    if (_is_running)
	return (XORP_OK);

    if (RoutingSocket::start(error_msg) < 0)
	return (XORP_ERROR);

    _is_running = true;

    return (XORP_OK);
}

int
IfConfigObserverRtsock::stop(string& error_msg)
{
    if (! _is_running)
	return (XORP_OK);

    if (RoutingSocket::stop(error_msg) < 0)
	return (XORP_ERROR);

    _is_running = false;

    return (XORP_OK);
}

void
IfConfigObserverRtsock::receive_data(const vector<uint8_t>& buffer)
{
    if (ifc().ifc_get_primary().parse_buffer_rtm(ifc().live_config(), buffer)
	!= true) {
	return;
    }

    ifc().report_updates(ifc().live_config(), true);

    // Propagate the changes from the live config to the local config
    IfTree& local_config = ifc().local_config();
    local_config.track_live_config_state(ifc().live_config());
    ifc().report_updates(local_config, false);
    local_config.finalize_state();
    ifc().live_config().finalize_state();
}

void
IfConfigObserverRtsock::rtsock_data(const vector<uint8_t>& buffer)
{
    receive_data(buffer);
}
