# Makefilepour le module hello
obj-m := statfs.o
KDIR := /lib/modules/$(shell uname -r)/build
KKDIR := /lib/modules/$(shell uname -r)/
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp statfs.ko $(KKDIR)
	depmod -a
clean:
	$(MAKE) -C  $(KDIR) M=$(PWD) clean
