# Context dependent makefile that can be called directly and will invoke itself
# through the kernel module building system.

ccflags-y := -std=gnu99 -Wno-declaration-after-statement

obj-m := secvault.o

KERNELDIR=/usr/src/linux

all: modules

modules modules_install clean:
	$(MAKE) V=1 ARCH=um -C $(KERNELDIR) M=$$PWD modules

#clean:
#	rm -rf Module.symvers .*.cmd *.ko .*.o *.o *.mod.c .tmp_versions *.order
