#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <net/genetlink.h> /* Needed for generic neltink */

#include "netlink_interface.h"

enum hello_multicast_groups {
	HELLO_MULTICAST_GROUP,
};

static const struct genl_multicast_group hello_mcgrps[] = {
	[HELLO_MULTICAST_GROUP] = { .name = "hellomcgrp" },
};

static const struct nla_policy hello_genl_policy[HELLO_ATTR_MAX + 1] = {
	[HELLO_ATTR_MESSAGE_0] = { .type = NLA_STRING, .len = 64 },
	[HELLO_ATTR_MESSAGE_1] = { .type = NLA_STRING, .len = 64 },
};

static int hello_trigger_event(struct sk_buff *skb, struct genl_info *info);
static int hello_say_hello(struct sk_buff *skb, struct genl_info *info);
static int hello_say_anything(struct sk_buff *skb, struct genl_info *info);

static const struct genl_ops hello_ops[] = {
	{
		.cmd = HELLO_CMD_SIMPLE,
		.doit = hello_cmd_simple,
	},
	{
		.cmd = HELLO_CMD_SAY_ANYTHING,
		.policy = hello_genl_policy,
		.doit = hello_say_anything,
	},
	{
		.cmd = HELLO_CMD_TRIGGER_EVENT,
		.policy = hello_genl_policy,
		.doit = hello_trigger_event,
	},
};

static struct genl_family hello_genl_family = {
	.module = THIS_MODULE,
	.name = "hellogenl",
	.version = 1,
	.maxattr = HELLO_ATTR_MAX,

	.ops = hello_ops,
	.n_ops = ARRAY_SIZE(hello_ops),

	.mcgrps = hello_mcgrps,
	.n_mcgrps = ARRAY_SIZE(hello_mcgrps),
};

static int hello_cmd_simple(struct sk_buff *skb,
			    struct genl_info *info)
{
	printk(KERN_DEBUG "Hello world!");
	return 0;
}

static int hello_say_anything(struct sk_buff *skb,
			      struct genl_info *info)
{
	const char *anything = NULL;
	struct sk_buff *mcast_skb = NULL;
	void *data;
	int ret;

	if (info->attrs[HELLO_ATTR_MESSAGE_0]) {
		anything = nla_data(info->attrs[HELLO_ATTR_MESSAGE_0]);
		printk(KERN_DEBUG "Hello %s!", anything);
	}

	mcast_skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!mcast_skb)
		return 0;

	data = genlmsg_put(mcast_skb, 0, 0, &hello_genl_family, 0,
			   HELLO_CMD_TRIGGER_EVENT);
	if (!data)
		goto out_err;

	ret = nla_put_string(mcast_skb, HELLO_ATTR_MESSAGE_0, "Reply!");
	if (ret)
		goto out_err;

	genlmsg_end(mcast_skb, data);

	genlmsg_reply(skb, info);

	return 0;

out_err:
	genlmsg_cancel(mcast_skb, data);
	nlmsg_free(mcast_skb);
	return -1;
}

static int hello_trigger_event(struct sk_buff *skb,
			       struct genl_info *info)
{
	struct sk_buff *mcast_skb = NULL;
	void *data;
	int ret;

	mcast_skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!mcast_skb)
		return 0;

	data = genlmsg_put(mcast_skb, 0, 0, &hello_genl_family, 0, HELLO_CMD_TRIGGER_EVENT);
	if (!data)
		goto out_err;

	ret = nla_put_string(mcast_skb, HELLO_ATTR_MESSAGE_0, "Hello event!");
	if (ret)
		goto out_err;

	genlmsg_end(mcast_skb, data);

	genlmsg_multicast(&hello_genl_family,
			  mcast_skb,
			  0,
			  HELLO_MULTICAST_GROUP,
			  GFP_KERNEL);
	return 0;
out_err:
	genlmsg_cancel(mcast_skb, data);
	nlmsg_free(mcast_skb);
	return -1;
}

static __init int hellogenl_init(void) 
{
	int err;

	err = genl_register_family(&hello_genl_family);
	if (err)
		return 1;
	return 0;
}

static __exit void hellogenl_exit(void)
{
	genl_unregister_family(&hello_genl_family);
}

module_init(hellogenl_init);
module_exit(hellogenl_exit);

MODULE_LICENSE("GPL");
