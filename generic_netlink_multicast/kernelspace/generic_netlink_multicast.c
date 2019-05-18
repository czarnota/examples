#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/timer.h>    /* Needed for timer_setup() */
#include <linux/workqueue.h>     /* Needed for schedule_work() */
#include <linux/completion.h> /* Needed for wait_for_completion() */
#include <net/genetlink.h> /* Needed for generic neltink */

#include "generic_netlink_multicast.h"

enum generic_netlink_multicast_groups {
	GENERIC_NETLINK_MULTICAST
};

static const struct genl_multicast_group generic_netlink_multicast_mcgrps[] = {
	[GENERIC_NETLINK_MULTICAST] = { .name = "gnm_group" }
};

static struct genl_family generic_netlink_multicast_genl_family = {
	.module = THIS_MODULE,
	.name = "gnm",
	.version = 1,

	.mcgrps = generic_netlink_multicast_mcgrps,
	.n_mcgrps = ARRAY_SIZE(generic_netlink_multicast_mcgrps)
};

struct generic_netlink_multicast {
	struct timer_list timer;
	struct work_struct work;
	struct completion completion;
	atomic_t stop;
};

static void work_handler(struct work_struct *work)
{
	struct generic_netlink_multicast *gnm =
		container_of(work, struct generic_netlink_multicast, work);
	struct sk_buff *skb;
	void *data;
	int ret;

	skb = genlmsg_new(GENLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return;

	data = genlmsg_put(skb, 0, 0, &generic_netlink_multicast_genl_family, 0,
			   GENERIC_NETLINK_MULTICAST_CMD);
	if (!data)
		goto out_err;

	ret = nla_put_string(skb, GENERIC_NETLINK_MULTICAST_ATTR_FOO, "Multicast Reply!");
	if (ret)
		goto out_err;

	ret = nla_put_u32(skb, GENERIC_NETLINK_MULTICAST_ATTR_BAR, 666);
	if (ret)
		goto out_err;

	ret = nla_put_u8(skb, GENERIC_NETLINK_MULTICAST_ATTR_BAZ, 13);
	if (ret)
		goto out_err;
	genlmsg_end(skb, data);
	genlmsg_multicast(&generic_netlink_multicast_genl_family, skb, 0,
			  GENERIC_NETLINK_MULTICAST,
			  GFP_KERNEL);

	printk(KERN_INFO "Sent multicast message...");
out_err:
	if (atomic_read(&gnm->stop))
		complete(&gnm->completion);
	else
		mod_timer(&gnm->timer, jiffies + HZ);
}

/* this function runs in softirq context*/
static void on_timer_tick(struct timer_list *t)
{
	struct generic_netlink_multicast *gnm = from_timer(gnm, t, timer);
	schedule_work(&gnm->work);
}

static struct generic_netlink_multicast gnm;

static __init int generic_netlink_multicast_init(void) 
{
	int err;

	timer_setup(&gnm.timer, on_timer_tick, 0);
	INIT_WORK(&gnm.work, work_handler);
	init_completion(&gnm.completion);
	atomic_set(&gnm.stop, 0);

	err = genl_register_family(&generic_netlink_multicast_genl_family);
	if (err)
		return 1;

	mod_timer(&gnm.timer, jiffies + HZ);

	return 0;
}

static __exit void generic_netlink_multicast_exit(void)
{
	atomic_set(&gnm.stop, 1);
	wait_for_completion(&gnm.completion);
	genl_unregister_family(&generic_netlink_multicast_genl_family);
}

module_init(generic_netlink_multicast_init);
module_exit(generic_netlink_multicast_exit);

MODULE_LICENSE("GPL");
