#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */


static __init int helloworld_init(void) 
{
    printk(KERN_INFO "Hello helloworld\n");
    return 0;
}


static __exit void helloworld_exit(void)
{
    printk(KERN_INFO "Goodbye helloworld\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
