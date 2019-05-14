#ifndef __generic_netlink_1_h__
#define __generic_netlink_1_h__

enum {
	GENERIC_NETLINK_1_ATTR_FOO,
	GENERIC_NETLINK_1_ATTR_BAR,
	GENERIC_NETLINK_1_ATTR_BAZ,

	__GENERIC_NETLINK_1_ATTR_COUNT
};
#define GENERIC_NETLINK_1_ATTR_MAX (__GENERIC_NETLINK_1_ATTR_COUNT - 1)

enum {
	GENERIC_NETLINK_1_CMD_WITHOUT_ARGS,
	GENERIC_NETLINK_1_CMD_WITH_ARGS,
	GENERIC_NETLINK_1_CMD_WITH_ARGS_AND_RESPONSE,

	__GENERIC_NETLINK_1_CMD_COUNT
};
#define GENERIC_NETLINK_1_CMD_MAX (__GENERIC_NETLINK_1_CMD_COUNT - 1)

#endif