TOPDIR=../../

.PHONY: all clean 

include $(TOPDIR)/misc/NaplesPU.mk

CFLAGS+=
VPATH=src

all: $(OBJ_DIR) libc.a

SRCS= ctype.c \
	math.c \
	memcpy.s \
	memset.c \
	memcmp.c \
	string.c \
	stdlib.c

OBJS := $(SRCS_TO_OBJS)
DEPS := $(SRCS_TO_DEPS)


libc.a: $(DEPS) $(OBJS)
	$(AR) r $@ $(OBJS)

clean:
	rm -rf $(OBJ_DIR)
	rm -f libc.a

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

-include $(DEPS)
