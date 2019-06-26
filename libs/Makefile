TOPDIR=../

.PHONY: all clean

include $(TOPDIR)/misc/NaplesPU.mk

all: crt0.o vectors.o
	cd libcompiler && make
	cd libc && make
	cd isr && make

OBJS := $(SRCS_TO_OBJS)

crt0.o: crt0.s
	$(CC) $(CFLAGS) -o $@ -c $<

vectors.o: vectors.s
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f crt0.o
	rm -f vectors.o
	cd libcompiler && make clean
	cd libc && make clean
	cd isr && make clean

