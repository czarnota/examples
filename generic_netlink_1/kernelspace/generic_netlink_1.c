#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <net/genetlink.h> /* Needed for generic neltink */

#include "generic_netlink_1.h"

static const struct nla_policy foo_policy[GENERIC_NETLINK_1_ATTR_MAX + 1] = {
	[GENERIC_NETLINK_1_ATTR_FOO] = { .type = NLA_UNSPEC },
	[GENERIC_NETLINK_1_ATTR_BAR] = { .type = NLA_U32 },
	[GENERIC_NETLINK_1_ATTR_BAZ] = { .type = NLA_U8 },
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
		.policy = foo_policy,
		.doit = generic_netlink_1_cmd_with_args
	},
	{
		.cmd = GENERIC_NETLINK_1_CMD_WITH_ARGS_AND_RESPONSE,
		.policy = foo_policy,
		.doit = generic_netlink_1_cmd_with_args_and_response
	},
};

static struct genl_family generic_netlink_1_genl_family = {
	.module = THIS_MODULE,
	.name = "gn_1",
	.version = 1,

	.maxattr = GENERIC_NETLINK_1_ATTR_MAX,

	.ops = generic_netlink_1_ops,
	.n_ops = ARRAY_SIZE(generic_netlink_1_ops),
};

static int generic_netlink_1_cmd_without_args(struct sk_buff *skb, struct genl_info *info)
{
	printk(KERN_INFO "Hello world!");
	return 0;
}

static int generic_netlink_1_cmd_with_args(struct sk_buff *skb, struct genl_info *info)
{
	const char *str;

	printk(KERN_INFO "foo: %p", info->attrs[GENERIC_NETLINK_1_ATTR_FOO]);
	if (info->attrs[GENERIC_NETLINK_1_ATTR_FOO]) {
		str = nla_data(info->attrs[GENERIC_NETLINK_1_ATTR_FOO]);
		printk(KERN_INFO "foo: %s", str);
	}

	if (info->attrs[GENERIC_NETLINK_1_ATTR_BAR])
		printk(KERN_INFO "bar: %u", nla_get_u32(info->attrs[GENERIC_NETLINK_1_ATTR_BAR]));

	if (info->attrs[GENERIC_NETLINK_1_ATTR_BAZ])
		printk(KERN_INFO "baz: %u", nla_get_u8(info->attrs[GENERIC_NETLINK_1_ATTR_BAZ]));

	return 0;
}

static int generic_netlink_1_cmd_with_args_and_response(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *new_skb;
	void *data;
	int ret;
	const char *str;

	printk(KERN_INFO "foo: %p", info->attrs[GENERIC_NETLINK_1_ATTR_FOO]);
	if (info->attrs[GENERIC_NETLINK_1_ATTR_FOO]) {
		str = nla_data(info->attrs[GENERIC_NETLINK_1_ATTR_FOO]);
		printk(KERN_INFO "foo: %s", str);
	}

	if (info->attrs[GENERIC_NETLINK_1_ATTR_BAR])
		printk(KERN_INFO "bar: %u", nla_get_u32(info->attrs[GENERIC_NETLINK_1_ATTR_BAR]));

	if (info->attrs[GENERIC_NETLINK_1_ATTR_BAZ])
		printk(KERN_INFO "baz: %u", nla_get_u8(info->attrs[GENERIC_NETLINK_1_ATTR_BAZ]));


	new_skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!new_skb)
		return 0;

	data = genlmsg_put(new_skb, info->snd_portid, info->snd_seq, &generic_netlink_1_genl_family, 0,
			   GENERIC_NETLINK_1_CMD_WITH_ARGS_AND_RESPONSE);
	if (!data)
		goto out_err;
	ret = nla_put_string(new_skb, GENERIC_NETLINK_1_ATTR_FOO, "Reply!");
	if (ret)
		goto out_err;
	ret = nla_put_u32(new_skb, GENERIC_NETLINK_1_ATTR_BAR, 666);
	if (ret)
		goto out_err;
	ret = nla_put_u8(new_skb, GENERIC_NETLINK_1_ATTR_BAZ, 13);
	if (ret)
		goto out_err;
	genlmsg_end(new_skb, data);

	genlmsg_reply(new_skb, info);

	return 0;
out_err:
	genlmsg_cancel(new_skb, data);
	nlmsg_free(new_skb);
	return -1;
}

static __init int generic_netlink_1_init(void) 
{
	int err;

	err = genl_register_family(&generic_netlink_1_genl_family);
	if (err)
		return 1;
	return 0;
}

static __exit void generic_netlink_1_exit(void)
{
	genl_unregister_family(&generic_netlink_1_genl_family);
}

module_init(generic_netlink_1_init);
module_exit(generic_netlink_1_exit);

MODULE_LICENSE("GPL");
