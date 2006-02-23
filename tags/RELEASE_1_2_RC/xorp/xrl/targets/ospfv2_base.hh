/*
 * Copyright (c) 2001-2005 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'tgt-gen'.
 *
 * $XORP: xorp/xrl/targets/ospfv2_base.hh,v 1.18 2006/02/15 19:06:15 pavlin Exp $
 */


#ifndef __XRL_INTERFACES_OSPFV2_BASE_HH__
#define __XRL_INTERFACES_OSPFV2_BASE_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XrlOspfv2Target"

#include "libxorp/xlog.h"
#include "libxipc/xrl_cmd_map.hh"

class XrlOspfv2TargetBase {
protected:
    XrlCmdMap* _cmds;

public:
    /**
     * Constructor.
     *
     * @param cmds an XrlCmdMap that the commands associated with the target
     *		   should be added to.  This is typically the XrlRouter
     *		   associated with the target.
     */
    XrlOspfv2TargetBase(XrlCmdMap* cmds = 0);

    /**
     * Destructor.
     *
     * Dissociates instance commands from command map.
     */
    virtual ~XrlOspfv2TargetBase();

    /**
     * Set command map.
     *
     * @param cmds pointer to command map to associate commands with.  This
     * argument is typically a pointer to the XrlRouter associated with the
     * target.
     *
     * @return true on success, false if cmds is null or a command map has
     * already been supplied.
     */
    bool set_command_map(XrlCmdMap* cmds);

    /**
     * Get Xrl instance name associated with command map.
     */
    inline const string& name() const { return _cmds->name(); }

    /**
     * Get version string of instance.
     */
    inline const char* version() const { return "ospfv2/0.0"; }

protected:

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get name of Xrl Target
     */
    virtual XrlCmdError common_0_1_get_target_name(
	// Output values,
	string&	name) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get version string from Xrl Target
     */
    virtual XrlCmdError common_0_1_get_version(
	// Output values,
	string&	version) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get status of Xrl Target
     */
    virtual XrlCmdError common_0_1_get_status(
	// Output values,
	uint32_t&	status,
	string&	reason) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Request clean shutdown of Xrl Target
     */
    virtual XrlCmdError common_0_1_shutdown() = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Receive an IPv4 packet from a raw socket.
     *
     *  @param if_name the interface name the packet arrived on.
     *
     *  @param vif_name the vif name the packet arrived on.
     *
     *  @param src_address the IP source address.
     *
     *  @param dst_address the IP destination address.
     *
     *  @param ip_protocol the IP protocol number.
     *
     *  @param ip_ttl the IP TTL (hop-limit). If it has a negative value, then
     *  the received value is unknown.
     *
     *  @param ip_tos the Type of Service (Diffserv/ECN bits for IPv4). If it
     *  has a negative value, then the received value is unknown.
     *
     *  @param ip_router_alert if true, the IP Router Alert option was included
     *  in the IP packet.
     */
    virtual XrlCmdError raw_packet4_client_0_1_recv(
	// Input values,
	const string&	if_name,
	const string&	vif_name,
	const IPv4&	src_address,
	const IPv4&	dst_address,
	const uint32_t&	ip_protocol,
	const int32_t&	ip_ttl,
	const int32_t&	ip_tos,
	const bool&	ip_router_alert,
	const vector<uint8_t>&	payload) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Configure a policy filter.
     *
     *  @param filter the identifier of the filter to configure.
     *
     *  @param conf the configuration of the filter.
     */
    virtual XrlCmdError policy_backend_0_1_configure(
	// Input values,
	const uint32_t&	filter,
	const string&	conf) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Reset a policy filter.
     *
     *  @param filter the identifier of the filter to reset.
     */
    virtual XrlCmdError policy_backend_0_1_reset(
	// Input values,
	const uint32_t&	filter) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Push all available routes through all filters for re-filtering.
     */
    virtual XrlCmdError policy_backend_0_1_push_routes() = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Start route redistribution for an IPv4 route.
     *
     *  @param network the route to advertise.
     *
     *  @param unicast whether the route is unicast.
     *
     *  @param multicast whether the route is multicast.
     *
     *  @param nexthop the nexthop of the route.
     *
     *  @param metric the metric of the route.
     *
     *  @param policytags the set of policy-tags associated with the route.
     */
    virtual XrlCmdError policy_redist4_0_1_add_route4(
	// Input values,
	const IPv4Net&	network,
	const bool&	unicast,
	const bool&	multicast,
	const IPv4&	nexthop,
	const uint32_t&	metric,
	const XrlAtomList&	policytags) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Terminate route redistribution for an IPv4 route.
     *
     *  @param network the route for which advertisements should cease.
     *
     *  @param unicast whether the route is unicast.
     *
     *  @param multicast whether the route is multicast.
     */
    virtual XrlCmdError policy_redist4_0_1_delete_route4(
	// Input values,
	const IPv4Net&	network,
	const bool&	unicast,
	const bool&	multicast) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Set router id
     */
    virtual XrlCmdError ospfv2_0_1_set_router_id(
	// Input values,
	const IPv4&	id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Set the router alert in the IP options.
     */
    virtual XrlCmdError ospfv2_0_1_set_ip_router_alert(
	// Input values,
	const bool&	ip_router_alert) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create an area.
     *
     *  @param area id of the area
     *
     *  @param type of area "border", "stub", "nssa"
     */
    virtual XrlCmdError ospfv2_0_1_create_area_router(
	// Input values,
	const IPv4&	area,
	const string&	type) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Change area type.
     *
     *  @param area id of the area
     *
     *  @param type of area "border", "stub", "nssa"
     */
    virtual XrlCmdError ospfv2_0_1_change_area_router_type(
	// Input values,
	const IPv4&	area,
	const string&	type) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Destroy area.
     *
     *  @param area id of the area
     */
    virtual XrlCmdError ospfv2_0_1_destroy_area_router(
	// Input values,
	const IPv4&	area) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create a binding to an interface.
     *
     *  @param ifname the interface that owns vif that has address.
     *
     *  @param vifname virtual interface owning address.
     *
     *  @param addr the address to be added.
     *
     *  @param prefix_len the prefix length XXX temporary.
     *
     *  @param mtu maximum transmission unit XXX temporary.
     *
     *  @param type of link "p2p", "broadcast", "nbma", "p2m", "vlink"
     */
    virtual XrlCmdError ospfv2_0_1_create_peer(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	addr,
	const uint32_t&	prefix_len,
	const uint32_t&	mtu,
	const string&	type,
	const IPv4&	area) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete peer.
     */
    virtual XrlCmdError ospfv2_0_1_delete_peer(
	// Input values,
	const string&	ifname,
	const string&	vifname) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Set the peer state up or down.
     */
    virtual XrlCmdError ospfv2_0_1_set_peer_state(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const bool&	enable) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Add a neighbour to the peer.
     */
    virtual XrlCmdError ospfv2_0_1_add_neighbour(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const IPv4&	neighbour_address,
	const IPv4&	neighbour_id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Remove a neighbour from the peer.
     */
    virtual XrlCmdError ospfv2_0_1_remove_neighbour(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const IPv4&	neighbour_address,
	const IPv4&	neighbour_id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create a virtual link.
     *
     *  @param neighbour_id the router ID of the other end of the link.
     *
     *  @param area in which an attempt has been made to configure a virtual
     *  link it has to be the backbone. Its just being passed in so it can be
     *  checked by the protocol.
     */
    virtual XrlCmdError ospfv2_0_1_create_virtual_link(
	// Input values,
	const IPv4&	neighbour_id,
	const IPv4&	area) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete virtual link
     *
     *  @param neighbour_id the router ID of the other end of the link.
     */
    virtual XrlCmdError ospfv2_0_1_delete_virtual_link(
	// Input values,
	const IPv4&	neighbour_id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  The area through which the virtual link transits.
     *
     *  @param neighbour_id the router ID of the other end of the link.
     *
     *  @param transit_area that the virtual link transits.
     */
    virtual XrlCmdError ospfv2_0_1_transit_area_virtual_link(
	// Input values,
	const IPv4&	neighbour_id,
	const IPv4&	transit_area) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  The edge cost of this interface.
     */
    virtual XrlCmdError ospfv2_0_1_set_interface_cost(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	cost) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  RxmtInterval The number of seconds between LSA retransmissions, for
     *  adjacencies belonging to this interface. Also used when retransmitting
     *  Database Description and Link State Request Packets. This should be
     *  well over the expected round-trip delay between any two routers on the
     *  attached network. The setting of this value should be conservative or
     *  needless retransmissions will result. Sample value for a local area
     *  network: 5 seconds.
     */
    virtual XrlCmdError ospfv2_0_1_set_retransmit_interval(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	interval) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  The estimated number of seconds it takes to transmit a Link State
     *  Update Packet over this interface. LSAs contained in the Link State
     *  Update packet will have their age incremented by this amount before
     *  transmission. This value should take into account transmission and
     *  propagation delays; it must be greater than zero.
     */
    virtual XrlCmdError ospfv2_0_1_set_inftransdelay(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	delay) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Used in the designated router election.
     */
    virtual XrlCmdError ospfv2_0_1_set_router_priority(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	priority) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  The interval between hello messages.
     */
    virtual XrlCmdError ospfv2_0_1_set_hello_interval(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	interval) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  The period to wait before considering a router dead.
     */
    virtual XrlCmdError ospfv2_0_1_set_router_dead_interval(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	interval) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Set simple password authentication key.
     *
     *  @param ifname the interface name.
     *
     *  @param vifname the vif name.
     *
     *  @param area the area ID.
     *
     *  @param password the authentication password.
     */
    virtual XrlCmdError ospfv2_0_1_set_simple_authentication_key(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const string&	password) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete simple password authentication key.
     *
     *  @param ifname the interface name.
     *
     *  @param vifname the vif name.
     *
     *  @param area the area ID.
     */
    virtual XrlCmdError ospfv2_0_1_delete_simple_authentication_key(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Set MD5 authentication key.
     *
     *  @param ifname the interface name.
     *
     *  @param vifname the vif name.
     *
     *  @param area the area ID.
     *
     *  @param key_id the key ID (must be an integer in the interval [0, 255]).
     *
     *  @param password the authentication password.
     *
     *  @param start_time the authentication start time (YYYY-MM-DD.HH:MM).
     *
     *  @param end_time the authentication end time (YYYY-MM-DD.HH:MM).
     */
    virtual XrlCmdError ospfv2_0_1_set_md5_authentication_key(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	key_id,
	const string&	password,
	const string&	start_time,
	const string&	end_time) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete MD5 authentication key.
     *
     *  @param ifname the interface name.
     *
     *  @param vifname the vif name.
     *
     *  @param area the area ID.
     *
     *  @param key_id the key ID (must be an integer in the interval [0, 255]).
     */
    virtual XrlCmdError ospfv2_0_1_delete_md5_authentication_key(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const uint32_t&	key_id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Toggle the passive status of an interface.
     */
    virtual XrlCmdError ospfv2_0_1_set_passive(
	// Input values,
	const string&	ifname,
	const string&	vifname,
	const IPv4&	area,
	const bool&	passive) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Add area range.
     */
    virtual XrlCmdError ospfv2_0_1_area_range_add(
	// Input values,
	const IPv4&	area,
	const IPv4Net&	net,
	const bool&	advertise) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete area range.
     */
    virtual XrlCmdError ospfv2_0_1_area_range_delete(
	// Input values,
	const IPv4&	area,
	const IPv4Net&	net) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Change the advertised state of this area.
     */
    virtual XrlCmdError ospfv2_0_1_area_range_change_state(
	// Input values,
	const IPv4&	area,
	const IPv4Net&	net,
	const bool&	advertise) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Enable/Disable tracing.
     *
     *  @param tvar trace variable.
     *
     *  @param enable set to true to enable false to disable.
     */
    virtual XrlCmdError ospfv2_0_1_trace(
	// Input values,
	const string&	tvar,
	const bool&	enable) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get a single lsa from an area. A stateless mechanism to get LSAs. The
     *  client of this interface should start from zero and continue to request
     *  LSAs (incrementing index) until toohigh becomes true.
     *
     *  @param area database that is being searched.
     *
     *  @param index into database starting from 0.
     *
     *  @param valid true if a LSA has been returned. Some index values do not
     *  contain LSAs. This should not be considered an error.
     *
     *  @param toohigh true if no more LSA exist after this index.
     *
     *  @param self if true this LSA was originated by this router.
     *
     *  @param lsa if valid is true the LSA at index.
     */
    virtual XrlCmdError ospfv2_0_1_get_lsa(
	// Input values,
	const IPv4&	area,
	const uint32_t&	index,
	// Output values,
	bool&	valid,
	bool&	toohigh,
	bool&	self,
	vector<uint8_t>&	lsa) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get a list of all the configured areas.
     */
    virtual XrlCmdError ospfv2_0_1_get_area_list(
	// Output values,
	XrlAtomList&	areas) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get the list of neighbours.
     */
    virtual XrlCmdError ospfv2_0_1_get_neighbour_list(
	// Output values,
	XrlAtomList&	areas) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Get information on a neighbour.
     *
     *  @param nid neighbour ID returned by the get_neighbour_list.
     *
     *  @param address of neighbour in txt to allow IPv4 and IPv6.
     *
     *  @param interface with which the neighbour forms the adjacency.
     *
     *  @param state of the adjacency.
     *
     *  @param rid router ID of the neighbour.
     *
     *  @param priority of the neighbour (used for DR election).
     *
     *  @param deadtime time until neighbour is considered dead.
     *
     *  @param area the neighbour is in.
     *
     *  @param opt value in the neighbours hello packet.
     *
     *  @param dr designated router.
     *
     *  @param bdr backup designated router.
     *
     *  @param up time in seconds that the neigbour has been up.
     *
     *  @param adjacent time in seconds that there has been an adjacency.
     */
    virtual XrlCmdError ospfv2_0_1_get_neighbour_info(
	// Input values,
	const uint32_t&	nid,
	// Output values,
	string&	address,
	string&	interface,
	string&	state,
	IPv4&	rid,
	uint32_t&	priority,
	uint32_t&	deadtime,
	IPv4&	area,
	uint32_t&	opt,
	IPv4&	dr,
	IPv4&	bdr,
	uint32_t&	up,
	uint32_t&	adjacent) = 0;

private:
    const XrlCmdError handle_common_0_1_get_target_name(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_get_version(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_get_status(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_shutdown(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_raw_packet4_client_0_1_recv(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_backend_0_1_configure(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_backend_0_1_reset(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_backend_0_1_push_routes(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_redist4_0_1_add_route4(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_redist4_0_1_delete_route4(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_router_id(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_ip_router_alert(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_create_area_router(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_change_area_router_type(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_destroy_area_router(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_create_peer(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_delete_peer(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_peer_state(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_add_neighbour(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_remove_neighbour(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_create_virtual_link(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_delete_virtual_link(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_transit_area_virtual_link(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_interface_cost(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_retransmit_interval(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_inftransdelay(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_router_priority(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_hello_interval(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_router_dead_interval(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_simple_authentication_key(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_delete_simple_authentication_key(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_md5_authentication_key(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_delete_md5_authentication_key(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_set_passive(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_area_range_add(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_area_range_delete(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_area_range_change_state(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_trace(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_get_lsa(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_get_area_list(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_get_neighbour_list(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_ospfv2_0_1_get_neighbour_info(const XrlArgs& in, XrlArgs* out);

    void add_handlers();
    void remove_handlers();
};

#endif /* __XRL_INTERFACES_OSPFV2_BASE_HH__ */
