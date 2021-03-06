#ifndef __generic_netlink_1_h__
#define __generic_netlink_1_h__

enum {
	GENERIC_NETLINK_MULTICAST_ATTR_UNSPEC,
	GENERIC_NETLINK_MULTICAST_ATTR_FOO,
	GENERIC_NETLINK_MULTICAST_ATTR_BAR,
	GENERIC_NETLINK_MULTICAST_ATTR_BAZ,

	__GENERIC_NETLINK_MULTICAST_ATTR_COUNT
};
#define GENERIC_NETLINK_MULTICAST_ATTR_MAX (__GENERIC_NETLINK_MULTICAST_ATTR_COUNT - 1)

enum {
	GENERIC_NETLINK_MULTICAST_CMD_UNSPEC,
	GENERIC_NETLINK_MULTICAST_CMD,
	__GENERIC_NETLINK_MULTICAST_CMD_COUNT
};
#define GENERIC_NETLINK_MULTICAST_CMD_MAX (__GENERIC_NETLINK_MULTICAST_CMD_COUNT - 1)

#endif
