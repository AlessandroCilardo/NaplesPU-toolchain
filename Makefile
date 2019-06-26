PREFIX=$(MANGO_ROOT)

.PHONY: all libs misc libs-header clean

all: misc libs-header libs

libs: 
	cd libs/ && make
	sudo mkdir -p $(PREFIX)/usr/local/llvm-npu/libs-npu
	sudo cp -r libs/* $(PREFIX)/usr/local/llvm-npu/libs-npu/

misc: 	
	sudo mkdir -p $(PREFIX)/usr/local/llvm-npu/misc-npu
	sudo cp -r misc/* $(PREFIX)/usr/local/llvm-npu/misc-npu/

libs-header:	
	sudo mkdir -p $(PREFIX)/usr/local/llvm-npu/libs-npu/libc
	sudo cp -r libs/libc/include $(PREFIX)/usr/local/llvm-npu/libs-npu/libc

clean: 
	cd libs/ && make clean
	sudo rm -rf $(PREFIX)/usr/local/llvm-npu/libs-npu
	sudo rm -rf $(PREFIX)/usr/local/llvm-npu/misc-npu
