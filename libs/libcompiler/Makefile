TOPDIR=../../

.PHONY: all clean

include $(TOPDIR)/misc/NaplesPU.mk

CFLAGS+=-g -Werror
VPATH=src

all: $(OBJ_DIR) libcompiler.a

SRCS=#divsi3.c \
	#divdi3.c \
	modsi3.c \
	#moddi3.c \
	udivsi3.c \
	#udivdi3.c \
	umodsi3.c \
	#umoddi3.c

SRCS=divsi3.c modsi3.c udivsi3.c umodsi3.c

OBJS := $(SRCS_TO_OBJS)
DEPS := $(SRCS_TO_DEPS)

libcompiler.a: $(DEPS) $(OBJS)
	$(AR) r $@ $(OBJS)

clean:
	rm -rf $(OBJ_DIR)
	rm -f libcompiler.a

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

-include $(DEPS)
