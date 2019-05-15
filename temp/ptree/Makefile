obj-m := dbfs_ptree.o

all :
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules;
	sudo insmod dbfs_ptree.ko

clean :
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean;
	sudo rmmod dbfs_ptree.ko
