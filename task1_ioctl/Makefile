KDIR ?= $(HOME)/embedded_lab1/kernel/linux-5.15
ARCH ?= arm
CROSS_COMPILE ?= arm-linux-gnueabihf-
obj-m += asgn1_driver.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) KCFLAGS="-march=armv7-a -mfpu=vfpv3 -mfloat-abi=hard" modules
	$(CROSS_COMPILE)gcc -static asgn1_user_test.c -o asgn1_user_test

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -f asgn1_user_test
