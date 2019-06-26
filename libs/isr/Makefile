TOPDIR=../../

include $(TOPDIR)/misc/NaplesPU.mk

CFLAGS+=
VPATH=src

SRCS= isr.c

OBJS := $(SRCS_TO_OBJS)
DEPS := $(SRCS_TO_DEPS)

all: $(OBJ_DIR) isr.a

isr.a: $(DEPS) $(OBJS)
	$(AR) r $@ $(OBJS)

clean:
	rm -rf $(OBJ_DIR)
	rm -f isr.a

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

-include $(DEPS)
