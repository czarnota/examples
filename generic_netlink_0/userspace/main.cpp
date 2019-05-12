#include <iostream>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include "generic_netlink_0.h"

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

	auto family_id = genl_ctrl_resolve(nl_sock, "gn_0");
	if (family_id < 0) {
		nl_socket_free(nl_sock);
		return -1;
	}

	ret = genl_send_simple(nl_sock, family_id, GENERIC_NETLINK_0_CMD, 1, 0);
	if (ret) {
		nl_socket_free(nl_sock);
		return -1;
	}

	return 0;
}
