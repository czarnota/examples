#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <net/genetlink.h> /* Needed for generic neltink */

#include "generic_netlink_1.h"

static const struct genl_policy foo_policy[GENERIC_NETLINK_1_ATTR_MAX + 1] = {
	[GENERIC_NETLINK_1_ATTR_FOO] = { .type = NLA_STRING, .len = 64 },
	[GENERIC_NETLINK_1_ATTR_BAR] = { .type = NLA_STRING, .len = 64 },
	[GENERIC_NETLINK_1_ATTR_BAZ] = { .type = NLA_STRING, .len = 64 },
};

static int generic_netlink_1_cmd_without_args(struct sk_buff *skb, struct genl_info *info);
static int generic_netlink_1_cmd_with_args(struct sk_buff *skb, struct genl_info *info);
static int generic_netlink_1_cmd_with_args_and_response(struct sk_buff *skb, struct genl_info *info);
static const struct genl_ops generic_netlink_1_ops[] = {
	{
		.cmd = GENERIC_NETLINK_1_CMD_WITHOUT_ARGS,
		.doit = generic_netlink_1_cmd_without_args
	},
	{
		.cmd = GENERIC_NETLINK_1_CMD_WITH_ARGS,
		.policy = &foo_policy,
		.doit = generic_netlink_1_cmd_with_args
	},
	{
		.cmd = GENERIC_NETLINK_1_CMD_WITH_ARGS_AND_RESPONCE,
		.policy = &foo_policy,
		.doit = generic_netlink_1_cmd_with_args_and_response
	},
};

static struct genl_family hello_genl_family = {
	.module = THIS_MODULE,
	.name = "gn_1",
	.version = 1,

	.maxattr = GENERIC_NETLINK_1_ATTR_MAX,

	.ops = generic_netlink_1_ops,
	.n_ops = ARRAY_SIZE(generic_netlink_1_ops),
};

static int generic_netlink_1_cmd_without_args(struct sk_buff *skb, struct genl_info *info);
{
	printk(KERN_INFO "Hello world!");
	return 0;
}

static int generic_netlink_1_cmd_with_args(struct sk_buff *skb, struct genl_info *info);
static int generic_netlink_1_cmd_with_args_and_response(struct sk_buff *skb, struct genl_info *info);

static __init int generic_netlink_1_init(void) 
{
	int err;

	err = genl_register_family(&hello_genl_family);
	if (err)
		return 1;
	return 0;
}

static __exit void generic_netlink_1_exit(void)
{
	genl_unregister_family(&hello_genl_family);
}

module_init(generic_netlink_1_init);
module_exit(generic_netlink_1_exit);

MODULE_LICENSE("GPL");
