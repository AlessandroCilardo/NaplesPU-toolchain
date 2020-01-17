cmake_minimum_required ( VERSION 3.1 )

project (NPU_isr)

set ( LLVM_NPU_DIR /usr/local/llvm-npu )
set ( LLVM_NPU_BIN_DIR ${LLVM_NPU_DIR}/bin )
set ( LIBC_INCLUDE_DIR ${LLVM_NPU_DIR}/libs-npu/libc/include )
set ( SRCS_DIR ${PROJECT_SOURCE_DIR}/src )
set ( INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )

file ( GLOB SRCS ${SRCS_DIR}/*.c* )
file ( GLOB INCS ${INCLUDE_DIR}/*.h* )

set ( CMAKE_C_COMPILER ${LLVM_NPU_BIN_DIR}/clang )
set ( CMAKE_AR ${LLVM_NPU_BIN_DIR}/llvm-ar )
set ( CMAKE_C_FLAGS "-O3 --target=naplespu -Wall -W" )

set (CMAKE_CXX_COMPILER ${LLVM_NPU_BIN_DIR}/clang++ )
set ( CMAKE_CXX_FLAGS "-O3 --target=naplespu -Wall -W" )

add_library ( isr STATIC ${SRCS} )

target_include_directories ( isr PUBLIC ${INCLUDE_DIR} ${LIBC_INCLUDE_DIR} )
set_target_properties(isr PROPERTIES PREFIX "" PUBLIC_HEADER "${INCS}")

install ( TARGETS isr ARCHIVE DESTINATION ${LLVM_NPU_DIR}/libs-npu/isr PUBLIC_HEADER DESTINATION  ${LLVM_NPU_DIR}/libs-npu/isr/include )
