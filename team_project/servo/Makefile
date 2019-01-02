KERNELDIR = /lib/modules/$(shell uname -r)/build

obj-m := servo.o

PWD := $(shell pwd)

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
