#include <iostream>
#include <functional>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include "generic_netlink_multicast.h"

int send_message(struct nl_sock *sock, int family_id, int cmd, 
		 std::function<int(struct nl_msg *msg)> filler)
{
	auto nlmsg = nlmsg_alloc();
	if (!nlmsg) {
		return -1;
	}

	auto data = genlmsg_put(nlmsg, 0, NL_AUTO_SEQ, family_id, 0, 0, cmd, 0);
	if (!data) {
		nlmsg_free(nlmsg);
		return -1;
	}

	auto ret = filler(nlmsg);
	if (ret) {
		nlmsg_free(nlmsg);
		return -1;
	}

	auto result = nl_send_auto(sock, nlmsg);
	nlmsg_free(nlmsg);
	if (result >= 0) {
		return 0;
	}
	return -1;
}

int on_response(struct nl_sock *sock, std::function<void(struct nl_msg *)> on_response)
{
	auto nl_cb = nl_socket_get_cb(sock);
	auto cb = nl_cb_clone(nl_cb);
	nl_cb_put(nl_cb);

	int err = 0;
	nl_cb_err(cb, NL_CB_CUSTOM, [] (auto nla, auto nlerr, auto arg) -> int {
		*static_cast<int *>(arg) = -1;
		return NL_STOP;
	}, static_cast<void *>(&err));

	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, [] (auto msg, auto arg) {
		return (*static_cast<std::function<int(struct nl_msg*)> *>(arg))(msg);
	}, static_cast<void*>(&on_response));

	auto ret = nl_recvmsgs(sock, cb);

	nl_cb_put(cb);

	if (err)
		return err;
	return ret;
}

int main(int argc, char **argv)
{
	int ret;

	auto nl_sock = nl_socket_alloc();
	if (!nl_sock) {
		return -1;
	}

	ret = genl_connect(nl_sock);

	if (ret) {
		nl_socket_free(nl_sock);
		return -1;
	}

	auto family_id = genl_ctrl_resolve(nl_sock, "gnm");
	if (family_id < 0) {
		nl_socket_free(nl_sock);
		return -1;
	}

	auto group_id = genl_ctrl_resolve_grp(nl_sock, "gnm", "gnm_group");
	if (group_id < 0) {
		nl_socket_free(nl_sock);
		return -1;
	}

	nl_socket_disable_seq_check(nl_sock);

	ret = nl_socket_add_membership(nl_sock, group_id);
	if (ret) {
		nl_socket_free(nl_sock);
		return -1;
	}


	/* Waits for answer - it will block() */
	while (!ret) {
		ret = on_response(nl_sock, [&] (auto msg) {
			struct nla_policy policy[GENERIC_NETLINK_MULTICAST_ATTR_MAX + 1];

			policy[GENERIC_NETLINK_MULTICAST_ATTR_FOO].type = NLA_STRING;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_FOO].minlen = 0;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_FOO].maxlen = 64;

			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAR].type = NLA_U32;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAR].minlen = 0;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAR].maxlen = 0;

			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAZ].type = NLA_U8;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAZ].minlen = 0;
			policy[GENERIC_NETLINK_MULTICAST_ATTR_BAZ].maxlen = 0;

			struct nlattr *attrs[GENERIC_NETLINK_MULTICAST_ATTR_MAX + 1];
			auto err = genlmsg_parse(nlmsg_hdr(msg), 0, attrs, GENERIC_NETLINK_MULTICAST_ATTR_MAX, policy);
			if (err < 0)
				return NL_STOP;

			auto header = static_cast<struct genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));

			if (header->cmd != GENERIC_NETLINK_MULTICAST_CMD) {
				return NL_STOP;
			}

			if (attrs[GENERIC_NETLINK_MULTICAST_ATTR_FOO])
				std::cout << nla_get_string(attrs[GENERIC_NETLINK_MULTICAST_ATTR_FOO]) << std::endl;

			if (attrs[GENERIC_NETLINK_MULTICAST_ATTR_BAR])
				std::cout << nla_get_u32(attrs[GENERIC_NETLINK_MULTICAST_ATTR_BAR]) << std::endl;

			if (attrs[GENERIC_NETLINK_MULTICAST_ATTR_BAZ])
				std::cout << static_cast<int>(nla_get_u8(attrs[GENERIC_NETLINK_MULTICAST_ATTR_BAZ])) << std::endl;

			return NL_STOP;
		});
	}

	nl_socket_free(nl_sock);
	return ret;
}
