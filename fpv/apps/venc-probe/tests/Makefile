CC ?= cc
CFLAGS ?= -O2 -g
CFLAGS += -std=c11 -Wall -Wextra -Werror -Iinclude -I..
.PHONY: all test clean
all: test_probe
test_probe: ../venc_probe.c test_probe.c ../venc_probe.h include/hi_mw_media_intf.h
	$(CC) $(CFLAGS) ../venc_probe.c test_probe.c -o $@
test: test_probe
	./test_probe
clean:
	rm -f test_probe
