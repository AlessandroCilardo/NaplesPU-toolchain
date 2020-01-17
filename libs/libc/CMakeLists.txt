cmake_minimum_required ( VERSION 3.1 )

project (NPU_libc)

set ( LLVM_NPU_DIR /usr/local/llvm-npu )
set ( LLVM_NPU_BIN_DIR ${LLVM_NPU_DIR}/bin )
set ( INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
set ( SRCS_DIR ${PROJECT_SOURCE_DIR}/src )

file ( GLOB SRCS ${SRCS_DIR}/*.c* )
file ( GLOB INCS ${INCLUDE_DIR}/*.h* )

set ( CMAKE_C_COMPILER ${LLVM_NPU_BIN_DIR}/clang )
set ( CMAKE_AR ${LLVM_NPU_BIN_DIR}/llvm-ar )
set ( CMAKE_C_FLAGS "-O3 --target=naplespu -Wall -W" )

set (CMAKE_CXX_COMPILER ${LLVM_NPU_BIN_DIR}/clang++ )
set ( CMAKE_CXX_FLAGS "-O3 --target=naplespu -Wall -W" )

add_library ( c STATIC ${SRCS} )

target_include_directories ( c PUBLIC ${INCLUDE_DIR} )

set_target_properties ( c PROPERTIES PUBLIC_HEADER "${INCS}" )

install ( TARGETS c ARCHIVE DESTINATION ${LLVM_NPU_DIR}/libs-npu/libc PUBLIC_HEADER DESTINATION ${LLVM_NPU_DIR}/libs-npu/libc/include )
