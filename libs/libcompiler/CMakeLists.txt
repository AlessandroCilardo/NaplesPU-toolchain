cmake_minimum_required ( VERSION 3.1 )

project (NPU_libcompiler)

set ( LLVM_NPU_DIR /usr/local/llvm-npu )
set ( LLVM_NPU_BIN_DIR ${LLVM_NPU_DIR}/bin )
set ( LIBC_INCLUDE_DIR ${LLVM_NPU_DIR}/libs-npu/libc/include )
set ( SRCS_DIR ${PROJECT_SOURCE_DIR}/src )

file ( GLOB SRCS ${SRCS_DIR}/*si3.c* )

set ( CMAKE_C_COMPILER ${LLVM_NPU_BIN_DIR}/clang )
set ( CMAKE_AR ${LLVM_NPU_BIN_DIR}/llvm-ar )
set ( CMAKE_C_FLAGS "-O3 --target=naplespu -Wall -W" )

set (CMAKE_CXX_COMPILER ${LLVM_NPU_BIN_DIR}/clang++ )
set ( CMAKE_CXX_FLAGS "-O3 --target=naplespu -Wall -W" )

add_library ( compiler STATIC ${SRCS} )

target_include_directories ( compiler PUBLIC ${LIBC_INCLUDE_DIR} )

install ( TARGETS compiler ARCHIVE DESTINATION ${LLVM_NPU_DIR}/libs-npu/libcompiler )
