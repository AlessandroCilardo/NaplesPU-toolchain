COMPILER_DIR=/usr/local/llvm-npu
ASM=$(COMPILER_DIR)/bin/clang
CFLAGS= -O3 --target=naplespu -W -Wall 
SRC_DIR=./src
SRCS=$(shell find ${SRC_DIR} -name *.s)
OBJS=$(SRCS:.s=.o)
OBJ_DIR=./build

build: $(OBJS)

%.o: %.s
	$(ASM) $(CFLAGS) -o $@ -c $<
	@mkdir -p $(OBJ_DIR) && mv $(SRC_DIR)/*.o $(OBJ_DIR) 

