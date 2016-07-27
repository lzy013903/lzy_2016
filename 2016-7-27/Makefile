obj-m := key_driver_lzy.o key_device_lzy.o

KDIR = /home/armlinux/kernel/linux-ok6410

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux- ARCH=arm

clean:
	rm -f *.ko *.o *.bak *.mod.c *.mod.o
