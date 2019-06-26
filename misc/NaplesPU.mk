BASE_DIR=$(MANGO_ROOT)/usr/local/llvm-npu
COMPILER_DIR=$(BASE_DIR)/bin
OBJ_DIR=obj

CC=$(COMPILER_DIR)/clang
CXX=$(COMPILER_DIR)/clang++ -std=c++11
LLC=$(COMPILER_DIR)/llc
LD=$(COMPILER_DIR)/ld.lld
AR=$(COMPILER_DIR)/llvm-ar
AS=$(COMPILER_DIR)/clang
OBJDUMP=$(COMPILER_DIR)/llvm-objdump
LLVMDIS=$(COMPILER_DIR)/llvm-dis
ELF2HEX=$(COMPILER_DIR)/elf2hex

OBJS= $(SRCS_TO_OBJS) $(NUP_DIR)/libs-npu/crt0.o $(NUP_DIR)/libs-npu/vectors.o
DEPS=$(SRCS_TO_DEPS)

CFLAGS=-O3 --target=naplespu -I$(BASE_DIR)/libs-npu/libc/include -Wall -W -DNPU_ACCELERATOR
LDFLAGS=-L$(BASE_DIR)/libs-npu/
AFLAGS=$(BASE_DIR)/libs-npu/libcompiler/libcompiler.a $(BASE_DIR)/libs-npu/libc/libc.a $(BASE_DIR)/libs-npu/isr/isr.a --script=$(BASE_DIR)/misc-npu/lnkrscrpt.ld

define SRCS_TO_OBJS
	$(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(foreach file, $(SRCS), $(basename $(notdir $(file))))))
endef

define SRCS_TO_DEPS
	$(addprefix $(OBJ_DIR)/, $(addsuffix .d, $(foreach file, $(filter-out %.s, $(SRCS)), $(basename $(notdir $(file))))))
endef

$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $(KFLAGS) -S -emit-llvm $< -o $(OBJ_DIR)/$(notdir $(basename $<)).ll
	$(CXX) $(CFLAGS) $(KFLAGS) -o $@ -c $< 


$(OBJ_DIR)/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(KFLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: %.s
	@echo "Assembling $<"
	@$(AS) --target=naplespu -o $@ -c $<

$(OBJ_DIR)/%.d: %.cpp
	@echo "Building dependencies for $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(KFLAGS) -o $(OBJ_DIR)/$*.d -M -MT $(OBJ_DIR)/$(notdir $(basename $<)).o $<

$(OBJ_DIR)/%.d: %.c
	@echo "Building dependencies for $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(KFLAGS) -o $(OBJ_DIR)/$*.d -M -MT $(OBJ_DIR)/$(notdir $(basename $<)).o $<


