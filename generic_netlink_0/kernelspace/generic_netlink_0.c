#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <net/genetlink.h> /* Needed for generic neltink */

#include "generic_netlink_0.h"

static int generic_netlink_0_cmd(struct sk_buff *skb, struct genl_info *info);
static const struct genl_ops generic_netlink_0_ops[] = {
	{
		.cmd = GENERIC_NETLINK_0_CMD,
		.doit = generic_netlink_0_cmd
	},
};

static struct genl_family hello_genl_family = {
	.module = THIS_MODULE,
	.name = "gn_0",
	.version = 1,

	.ops = generic_netlink_0_ops,
	.n_ops = ARRAY_SIZE(generic_netlink_0_ops),
};

static int generic_netlink_0_cmd(struct sk_buff *skb, struct genl_info *info)
{
	printk(KERN_INFO "Hello world!");
	return 0;
}

static __init int generic_netlink_0_init(void) 
{
	int err;

	err = genl_register_family(&hello_genl_family);
	if (err)
		return 1;
	return 0;
}

static __exit void generic_netlink_0_exit(void)
{
	genl_unregister_family(&hello_genl_family);
}

module_init(generic_netlink_0_init);
module_exit(generic_netlink_0_exit);

MODULE_LICENSE("GPL");
