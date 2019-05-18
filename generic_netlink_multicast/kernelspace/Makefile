obj-m += generic_netlink_multicast.o 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

reload: all
	sudo rmmod generic_netlink_multicast || true
	sudo insmod generic_netlink_multicast.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
