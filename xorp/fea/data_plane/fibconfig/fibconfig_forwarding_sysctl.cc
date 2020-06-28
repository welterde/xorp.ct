// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
// vim:set sts=4 ts=8:

// Copyright (c) 2001-2009 XORP, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, Version 2, June
// 1991 as published by the Free Software Foundation. Redistribution
// and/or modification of this program under the terms of any other
// version of the GNU General Public License is not permitted.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. For more details,
// see the GNU General Public License, Version 2, a copy of which can be
// found in the XORP LICENSE.gpl file.
// 
// XORP Inc, 2953 Bunker Hill Lane, Suite 204, Santa Clara, CA 95054, USA;
// http://xorp.net



#include "fea/fea_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"

#include "libcomm/comm_api.h"

#ifdef HAVE_LINUX_SYSCTL_H
#include <linux/sysctl.h>
#elif HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif


#include "fea/fibconfig.hh"

#include "fibconfig_forwarding_sysctl.hh"

//
// Configure unicast forwarding.
//
// The mechanism to get/set the information is sysctl(3).
//

#if defined(HAVE_SYSCTL_IPCTL_FORWARDING) || defined(HAVE_SYSCTL_IPV6CTL_FORWARDING) || defined(HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV)

FibConfigForwardingSysctl::FibConfigForwardingSysctl(
    FeaDataPlaneManager& fea_data_plane_manager)
    : FibConfigForwarding(fea_data_plane_manager)
{
}

FibConfigForwardingSysctl::~FibConfigForwardingSysctl()
{
}

int
FibConfigForwardingSysctl::unicast_forwarding_enabled4(bool& ret_value,
						       string& error_msg) const
{
#ifndef HAVE_SYSCTL_IPCTL_FORWARDING
    ret_value = false;
    error_msg = c_format("Cannot test whether IPv4 unicast forwarding "
			 "is enabled: sysctl(IPCTL_FORWARDING) is not supported");
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPCTL_FORWARDING

    int enabled = 0;

    if (! fea_data_plane_manager().have_ipv4()) {
	ret_value = false;
	error_msg = c_format("Cannot test whether IPv4 unicast forwarding "
			     "is enabled: IPv4 is not supported");
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Read the value from the MIB
    //
    size_t sz = sizeof(enabled);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET;
    mib[2] = IPPROTO_IP;
    mib[3] = IPCTL_FORWARDING;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), &enabled, &sz, NULL, 0)
	!= 0) {
	error_msg = c_format("Get sysctl(IPCTL_FORWARDING) failed: %s",
			     strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	    return (XORP_ERROR);
    }
    
    if (enabled > 0)
	ret_value = true;
    else
	ret_value = false;
    
    return (XORP_OK);
#endif // HAVE_SYSCTL_IPCTL_FORWARDING
}

int
FibConfigForwardingSysctl::unicast_forwarding_enabled6(bool& ret_value,
						       string& error_msg) const
{
#ifndef HAVE_SYSCTL_IPV6CTL_FORWARDING
    ret_value = false;
    error_msg = c_format("Cannot test whether IPv6 unicast forwarding "
			 "is enabled: sysctl(IPV6CTL_FORWARDING) is not supported");
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPV6CTL_FORWARDING

    int enabled = 0;

    if (! fea_data_plane_manager().have_ipv6()) {
	ret_value = false;
	error_msg = c_format("Cannot test whether IPv6 unicast forwarding "
			     "is enabled: IPv6 is not supported");
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Read the value from the MIB
    //
    size_t sz = sizeof(enabled);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET6;
    mib[2] = IPPROTO_IPV6;
    mib[3] = IPV6CTL_FORWARDING;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), &enabled, &sz, NULL, 0)
	!= 0) {
	error_msg = c_format("Get sysctl(IPV6CTL_FORWARDING) failed: %s",
			     strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    if (enabled > 0)
	ret_value = true;
    else
	ret_value = false;
    
    return (XORP_OK);
#endif // HAVE_SYSCTL_IPV6CTL_FORWARDING
}

int
FibConfigForwardingSysctl::accept_rtadv_enabled6(bool& ret_value,
						 string& error_msg) const
{
#ifndef HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV
    ret_value = false;
    error_msg = c_format("Cannot test whether the acceptance of IPv6 "
			 "Router Advertisement messages is enabled: "
			 "sysctl(IPV6CTL_ACCEPT_RTADV) is not supported");
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV

    int enabled = 0;

    if (! fea_data_plane_manager().have_ipv6()) {
	ret_value = false;
	error_msg = c_format("Cannot test whether the acceptance of IPv6 "
			     "Router Advertisement messages is enabled: "
			     "IPv6 is not supported");
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Read the value from the MIB
    //
    size_t sz = sizeof(enabled);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET6;
    mib[2] = IPPROTO_IPV6;
    mib[3] = IPV6CTL_ACCEPT_RTADV;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), &enabled, &sz, NULL, 0)
	!= 0) {
	error_msg = c_format("Get sysctl(IPV6CTL_ACCEPT_RTADV) failed: %s",
			     strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    if (enabled > 0)
	ret_value = true;
    else
	ret_value = false;

    return (XORP_OK);
#endif // HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV
}

int
FibConfigForwardingSysctl::set_unicast_forwarding_enabled4(bool v,
							   string& error_msg)
{
#ifndef HAVE_SYSCTL_IPCTL_FORWARDING
    error_msg = c_format("Cannot set IPv4 unicast forwarding to %s: "
			 "sysctl(IPCTL_FORWARDING) is not supported",
			 bool_c_str(v));
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPCTL_FORWARDING

    int enable = (v) ? 1 : 0;
    bool old_value;

    if (! fea_data_plane_manager().have_ipv4()) {
	if (! v) {
	    //
	    // XXX: we assume that "not supported" == "disable", hence
	    // return OK.
	    //
	    return (XORP_OK);
	}
	error_msg = c_format("Cannot set IPv4 unicast forwarding to %s: "
			     "IPv4 is not supported", bool_c_str(v));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Get the old value
    //
    if (unicast_forwarding_enabled4(old_value, error_msg) != XORP_OK)
	return (XORP_ERROR);
    if (old_value == v)
	return (XORP_OK);	// Nothing changed

    //
    // Write the value to the MIB
    //
    size_t sz = sizeof(enable);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET;
    mib[2] = IPPROTO_IP;
    mib[3] = IPCTL_FORWARDING;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, NULL, &enable, sz)
	!= 0) {
	error_msg = c_format("Set sysctl(IPCTL_FORWARDING) to %s failed: %s",
			     bool_c_str(v), strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    return (XORP_OK);
#endif // HAVE_SYSCTL_IPCTL_FORWARDING
}

int
FibConfigForwardingSysctl::set_unicast_forwarding_enabled6(bool v,
							   string& error_msg)
{
#ifndef HAVE_SYSCTL_IPV6CTL_FORWARDING
    error_msg = c_format("Cannot set IPv6 unicast forwarding to %s: "
			 "sysctl(IPV6CTL_FORWARDING) is not supported",
			 bool_c_str(v));
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPV6CTL_FORWARDING

    int enable = (v) ? 1 : 0;
    bool old_value, old_value_accept_rtadv;

    if (! fea_data_plane_manager().have_ipv6()) {
	if (! v) {
	    //
	    // XXX: we assume that "not supported" == "disable", hence
	    // return OK.
	    //
	    return (XORP_OK);
	}

	error_msg = c_format("Cannot set IPv6 unicast forwarding to %s: "
			     "IPv6 is not supported", bool_c_str(v));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Get the old value
    //
    if (unicast_forwarding_enabled6(old_value, error_msg) != XORP_OK)
	return (XORP_ERROR);
    if (accept_rtadv_enabled6(old_value_accept_rtadv, error_msg) != XORP_OK)
	return (XORP_ERROR);
    if ((old_value == v) && (old_value_accept_rtadv == !v))
	return (XORP_OK);	// Nothing changed

    //
    // Set the IPv6 Router Advertisement value
    //
    if (set_accept_rtadv_enabled6(!v, error_msg) != XORP_OK)
	return (XORP_ERROR);

    //
    // Write the value to the MIB
    //
    size_t sz = sizeof(enable);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET6;
    mib[2] = IPPROTO_IPV6;
    mib[3] = IPV6CTL_FORWARDING;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, NULL, &enable, sz)
	!= 0) {
	error_msg = c_format("Set sysctl(IPV6CTL_FORWARDING) to %s failed: %s",
			     bool_c_str(v), strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	// Restore the old accept_rtadv value
	if (old_value_accept_rtadv != !v) {
	    string dummy_error_msg;
	    set_accept_rtadv_enabled6(old_value_accept_rtadv, dummy_error_msg);
	}
	return (XORP_ERROR);
    }

    return (XORP_OK);
#endif // HAVE_SYSCTL_IPV6CTL_FORWARDING
}

int
FibConfigForwardingSysctl::set_accept_rtadv_enabled6(bool v, string& error_msg)
{
#ifndef HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV
    error_msg = c_format("Cannot set the acceptance of IPv6 "
			 "Router Advertisement messages to %s: "
			 "sysctl(IPV6CTL_ACCEPT_RTADV) is not supported",
			 bool_c_str(v));
    XLOG_ERROR("%s", error_msg.c_str());
    return (XORP_ERROR);

#else // HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV

    int enable = (v) ? 1 : 0;
    bool old_value;

    if (! fea_data_plane_manager().have_ipv6()) {
	if (! v) {
	    //
	    // XXX: we assume that "not supported" == "disable", hence
	    // return OK.
	    //
	    return (XORP_OK);
	}

	error_msg = c_format("Cannot set the acceptance of IPv6 "
			     "Router Advertisement messages to %s: "
			     "IPv6 is not supported",
			     bool_c_str(v));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    //
    // Get the old value
    //
    if (accept_rtadv_enabled6(old_value, error_msg) != XORP_OK)
	return (XORP_ERROR);
    if (old_value == v)
	return (XORP_OK);	// Nothing changed

    //
    // Write the value to the MIB
    //
    size_t sz = sizeof(enable);
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = AF_INET6;
    mib[2] = IPPROTO_IPV6;
    mib[3] = IPV6CTL_ACCEPT_RTADV;
    if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, NULL, &enable, sz)
	!= 0) {
	error_msg = c_format("Set sysctl(IPV6CTL_ACCEPT_RTADV) to %s failed: %s",
			     bool_c_str(v), strerror(errno));
	XLOG_ERROR("%s", error_msg.c_str());
	return (XORP_ERROR);
    }

    return (XORP_OK);

#endif // HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV
}

#endif // HAVE_SYSCTL_IPCTL_FORWARDING || HAVE_SYSCTL_IPV6CTL_FORWARDING || HAVE_SYSCTL_IPV6CTL_ACCEPT_RTADV
