#pragma once

#define __scratchpad __attribute__((scratchpad)) 

typedef char vec16i8 __attribute__((ext_vector_type(16)));
typedef unsigned char vec16u8 __attribute__((ext_vector_type(16)));
typedef short vec16i16 __attribute__((ext_vector_type(16)));
typedef unsigned short vec16u16 __attribute__((ext_vector_type(16)));
typedef int vec16i32 __attribute__((ext_vector_type(16)));
typedef unsigned int vec16u32 __attribute__((ext_vector_type(16)));
typedef float vec16f32 __attribute__((ext_vector_type(16)));

typedef char vec8i8 __attribute__((ext_vector_type(8)));
typedef unsigned char vec8u8 __attribute__((ext_vector_type(8)));
typedef short vec8i16 __attribute__((ext_vector_type(8)));
typedef unsigned short vec8u16 __attribute__((ext_vector_type(8)));
typedef int vec8i32 __attribute__((ext_vector_type(8)));
typedef unsigned int vec8u32 __attribute__((ext_vector_type(8)));
typedef long long int vec8i64 __attribute__((ext_vector_type(8)));
typedef unsigned long long int vec8u64 __attribute__((ext_vector_type(8)));
typedef double vec8f64 __attribute__((ext_vector_type(8)));

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
typedef unsigned int intptr_t;
