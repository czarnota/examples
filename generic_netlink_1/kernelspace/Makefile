obj-m += generic_netlink_1.o 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

reload: all
	sudo rmmod generic_netlink_1 || true
	sudo insmod generic_netlink_1.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
