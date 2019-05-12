#ifndef __netlink_interface_h__
#define __netlink_interface_h__

enum hello_attrs {
	HELLO_ATTR_MESSAGE_0,
	HELLO_ATTR_MESSAGE_1,

	__HELLO_ATTR_COUNT
};
#define HELLO_ATTR_MAX (__HELLO_ATTR_COUNT - 1)

enum hello_commands {
	HELLO_CMD_SIMPLE,
	HELLO_CMD_SAY_ANYTHING,
	HELLO_CMD_TRIGGER_EVENT,

	__HELLO_CMD_COUNT
};
#define HELLO_CMD_MAX (__HELLO_CMD_COUNT - 1)

#endif
