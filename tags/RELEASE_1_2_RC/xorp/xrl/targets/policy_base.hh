/*
 * Copyright (c) 2001-2005 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'tgt-gen'.
 *
 * $XORP: xorp/xrl/targets/policy_base.hh,v 1.9 2005/10/02 22:21:58 abittau Exp $
 */


#ifndef __XRL_INTERFACES_POLICY_BASE_HH__
#define __XRL_INTERFACES_POLICY_BASE_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XrlPolicyTarget"

#include "libxorp/xlog.h"
#include "libxipc/xrl_cmd_map.hh"

class XrlPolicyTargetBase {
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
    XrlPolicyTargetBase(XrlCmdMap* cmds = 0);

    /**
     * Destructor.
     *
     * Dissociates instance commands from command map.
     */
    virtual ~XrlPolicyTargetBase();

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
    inline const char* version() const { return "policy/0.0"; }

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
     *  Announce target birth to observer.
     *
     *  @param target_class the target class name.
     *
     *  @param target_instance the target instance name.
     */
    virtual XrlCmdError finder_event_observer_0_1_xrl_target_birth(
	// Input values,
	const string&	target_class,
	const string&	target_instance) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Announce target death to observer.
     *
     *  @param target_class the target class name.
     *
     *  @param target_instance the target instance name.
     */
    virtual XrlCmdError finder_event_observer_0_1_xrl_target_death(
	// Input values,
	const string&	target_class,
	const string&	target_instance) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create a term in a policy. The term will be the last term in the policy
     *  statement.
     *
     *  @param policy the name of the policy.
     *
     *  @param order position of term.
     *
     *  @param term the name of the term.
     */
    virtual XrlCmdError policy_0_1_create_term(
	// Input values,
	const string&	policy,
	const string&	order,
	const string&	term) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete a term in a policy.
     *
     *  @param policy the name of the policy.
     *
     *  @param term the name of the term.
     */
    virtual XrlCmdError policy_0_1_delete_term(
	// Input values,
	const string&	policy,
	const string&	term) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Update the source/dest/action block of a term in a policy. Note: if
     *  statement is empty, the previous one in that position will be deleted.
     *
     *  @param policy the name of the policy.
     *
     *  @param term the name of the term.
     *
     *  @param block the block to update (0:source, 1:dest, 2:action).
     *
     *  @param order numerical position (local) of statement.
     *
     *  @param statement the statement to insert or delete.
     */
    virtual XrlCmdError policy_0_1_update_term_block(
	// Input values,
	const string&	policy,
	const string&	term,
	const uint32_t&	block,
	const string&	order,
	const string&	statement) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create a policy.
     *
     *  @param policy name of the policy.
     */
    virtual XrlCmdError policy_0_1_create_policy(
	// Input values,
	const string&	policy) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete a policy.
     *
     *  @param policy name of the policy.
     */
    virtual XrlCmdError policy_0_1_delete_policy(
	// Input values,
	const string&	policy) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Create a set.
     *
     *  @param set name of the set.
     */
    virtual XrlCmdError policy_0_1_create_set(
	// Input values,
	const string&	set) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Replace elements in a set.
     *
     *  @param type the type of the set.
     *
     *  @param set name of the set.
     *
     *  @param elements comma delimited list of set elements.
     */
    virtual XrlCmdError policy_0_1_update_set(
	// Input values,
	const string&	type,
	const string&	set,
	const string&	elements) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Delete a set.
     *
     *  @param set name of the set.
     */
    virtual XrlCmdError policy_0_1_delete_set(
	// Input values,
	const string&	set) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Commit the configuration.
     */
    virtual XrlCmdError policy_0_1_done_global_policy_conf() = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Replace import/export policies for a protocol. Will normally cause a
     *  delayed commit.
     *
     *  @param protocol protocol for which policies must be replaced.
     *
     *  @param policies comma separated list of policies.
     */
    virtual XrlCmdError policy_0_1_import(
	// Input values,
	const string&	protocol,
	const string&	policies) = 0;

    virtual XrlCmdError policy_0_1_export(
	// Input values,
	const string&	protocol,
	const string&	policies) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Add a variable to the varmap configuration.
     *
     *  @param protocol the protocol this variable is available to.
     *
     *  @param variable name of the variable.
     *
     *  @param type the type of the variable.
     *
     *  @param access the permissions on the variable (r/rw).
     *
     *  @param id unique id for this var. Scoped by protocol.
     */
    virtual XrlCmdError policy_0_1_add_varmap(
	// Input values,
	const string&	protocol,
	const string&	variable,
	const string&	type,
	const string&	access,
	const uint32_t&	id) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Dump state of policy manager. To be used ONLY for debugging.
     *
     *  @param id which aspect of the internal state to dump.
     */
    virtual XrlCmdError policy_0_1_dump_state(
	// Input values,
	const uint32_t&	id,
	// Output values,
	string&	state) = 0;

    /**
     *  Pure-virtual function that needs to be implemented to:
     *
     *  Update the mapping from protocols to their XRL target name.
     *
     *  @param protocol protocol to update.
     *
     *  @param target XRL target for the protocol.
     */
    virtual XrlCmdError policy_0_1_set_proto_target(
	// Input values,
	const string&	protocol,
	const string&	target) = 0;

private:
    const XrlCmdError handle_common_0_1_get_target_name(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_get_version(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_get_status(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_common_0_1_shutdown(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_finder_event_observer_0_1_xrl_target_birth(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_finder_event_observer_0_1_xrl_target_death(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_create_term(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_delete_term(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_update_term_block(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_create_policy(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_delete_policy(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_create_set(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_update_set(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_delete_set(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_done_global_policy_conf(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_import(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_export(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_add_varmap(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_dump_state(const XrlArgs& in, XrlArgs* out);

    const XrlCmdError handle_policy_0_1_set_proto_target(const XrlArgs& in, XrlArgs* out);

    void add_handlers();
    void remove_handlers();
};

#endif /* __XRL_INTERFACES_POLICY_BASE_HH__ */
