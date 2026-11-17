# Cross-compile the VENC callback object against real HiSilicon SDK headers.
# No link or sensor startup: this is an ABI/compiler smoke test only.
ROOT := ../../..
CC ?= arm-himix100-linux-gcc
SDK_INCLUDE := $(ROOT)/middleware/media_adpt/include
MPP_INCLUDE := $(ROOT)/amp/a7_linux/mpp/include
CPPFLAGS += -I$(SDK_INCLUDE) -I$(MPP_INCLUDE)
CFLAGS ?= -O2 -Wall -Wextra
.PHONY: all clean
all: venc_probe.o
venc_probe.o: venc_probe.c venc_probe.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c venc_probe.c -o $@
clean:
	rm -f venc_probe.o
