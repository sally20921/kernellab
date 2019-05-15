KDIR = /lib/modules/$(shell uname -r)/build

obj-m := dbfs_paddr.o

all : 
	$(MAKE) -C $(KDIR) M=$(PWD) modules;
	gcc -o app app.c;
	sudo insmod dbfs_paddr.ko

clean : 
	$(MAKE) -C $(KDIR) M=$(PWD) clean;
	rm app;
	sudo rmmod dbfs_paddr.ko
