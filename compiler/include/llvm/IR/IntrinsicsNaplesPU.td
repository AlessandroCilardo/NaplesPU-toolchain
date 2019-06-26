//===- IntrinsicsNaplesPU.td - Defines NaplesPU intrinsics ---*- tablegen -*-===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This file defines all of the NaplesPU-specific intrinsics.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// NaplesPU data types

def v16i32_ptr_ty : LLVMPointerType<LLVMType<v16i32>>;

//===----------------------------------------------------------------------===//
// Definitions for all NaplesPU intrinsics.
//

let TargetPrefix = "npu" in {  // All intrinsics start with "llvm.npu.".

// Shuffle vector elements
def int_npu_shufflei32 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_shufflei32">;
def int_npu_shufflef32 : Intrinsic<[llvm_v16f32_ty], [llvm_v16f32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_shufflef32">;
def int_npu_shufflei64 : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_shufflei64">;
def int_npu_shufflef64 : Intrinsic<[llvm_v8f64_ty], [llvm_v8f64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_shufflef64">;

// The blend pattern is a pseudo-instruction used to encode masked operations
def int_npu_vector_mixi32 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty], 
	[IntrNoMem], "llvm.npu.__builtin_npu_vector_mixi32">;
def int_npu_vector_mixf32 : Intrinsic<[llvm_v16f32_ty], [llvm_v16f32_ty, llvm_v16f32_ty], 
	[IntrNoMem], "llvm.npu.__builtin_npu_vector_mixf32">;
def int_npu_vector_mixi64 : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty, llvm_v8i64_ty], 
	[IntrNoMem], "llvm.npu.__builtin_npu_vector_mixi64">;
def int_npu_vector_mixf64 : Intrinsic<[llvm_v8f64_ty], [llvm_v8f64_ty, llvm_v8f64_ty], 
	[IntrNoMem], "llvm.npu.__builtin_npu_vector_mixf64">;
	
def int_npu_v16f32tov8f64 : Intrinsic<[llvm_v8f64_ty], [llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_v16f32tov8f64">;
def int_npu_v8f64tov16f32 : Intrinsic<[llvm_v16f32_ty], [llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_v8f64tov16f32">;
	
// Cross Product Intrinsic
def int_npu_crossprodv16i32 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty], 
	[IntrNoMem], "llvm.npu.__builtin_npu_crossprodv16i32">;

//--------------------------------------------------------------------------------//
// Special registers
//--------------------------------------------------------------------------------//

//------ Create Mask ------//
// v16i32
def int_npu_createmaskv16i32 : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty],
	[], "llvm.npu.__builtin_npu_createmaskv16i32">;
// v8i64
def int_npu_createmaskv8i64 : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty],
	[], "llvm.npu.__builtin_npu_createmaskv8i64">;

//------ MR Load/Store ------//
// Read MR
def int_npu_read_mask_reg : Intrinsic<[llvm_i32_ty], [],
	[], "llvm.npu.__builtin_npu_read_mask_reg">;

// Write MR
def int_npu_write_mask_reg : Intrinsic<[], [llvm_i32_ty],
	[], "llvm.npu.__builtin_npu_write_mask_reg">;
def int_npu_write_mask_regv16i32 : Intrinsic<[], [llvm_v16i32_ty],
	[], "llvm.npu.__builtin_npu_write_mask_regv16i32">;
def int_npu_write_mask_regv8i64 : Intrinsic<[], [llvm_v8i64_ty],
	[], "llvm.npu.__builtin_npu_write_mask_regv8i64">;
  
//------ CR Load/Store ------//
// Read CR
def int_npu_read_control_reg : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [],
	"llvm.npu.__builtin_npu_read_control_reg">;

// Write CR
def int_npu_write_control_reg : Intrinsic<[], [llvm_i32_ty, llvm_i32_ty], [],
	"llvm.npu.__builtin_npu_write_control_reg">;

//--------------------------------------------------------------------------------//
// Memory operations
//--------------------------------------------------------------------------------//

//------ Vector load block ------//

//Opcode: 7
//-v16 32-//
def int_npu_block_loadv16i8 : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i8">;
def int_npu_block_loadv16i8_scratchpad : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i8_scratchpad">;
def int_npu_block_loadv16i8_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i8_masked">;
def int_npu_block_loadv16i8_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i8_scratchpad_masked">;
//-v8 64-//
def int_npu_block_loadv8i8 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i8">;
def int_npu_block_loadv8i8_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i8_scratchpad">;
def int_npu_block_loadv8i8_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i8_masked">;
def int_npu_block_loadv8i8_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i8_scratchpad_masked">;

//Opcode: 8
//-v16 32-//
def int_npu_block_loadv16i16 : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i16">;
def int_npu_block_loadv16i16_scratchpad : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i16_scratchpad">;
def int_npu_block_loadv16i16_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i16_masked">;
def int_npu_block_loadv16i16_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i16_scratchpad_masked">;
//-v8 64-//
def int_npu_block_loadv8i16 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i16">;
def int_npu_block_loadv8i16_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i16_scratchpad">;
def int_npu_block_loadv8i16_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i16_masked">;
def int_npu_block_loadv8i16_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i16_scratchpad_masked">;

//Opcode: 9
//-v16 32-//
def int_npu_block_loadv16i32_scratchpad : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i32_scratchpad">;
def int_npu_block_loadv16f32_scratchpad : Intrinsic<[llvm_v16f32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16f32_scratchpad">;
def int_npu_block_loadv16i32_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i32_masked">;
def int_npu_block_loadv16f32_masked : Intrinsic<[llvm_v16f32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16f32_masked">;
def int_npu_block_loadv16i32_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16i32_scratchpad_masked">;
def int_npu_block_loadv16f32_scratchpad_masked : Intrinsic<[llvm_v16f32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16f32_scratchpad_masked">;
//-v8 64-//
def int_npu_block_loadv8i32 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i32">;
def int_npu_block_loadv8i32_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i32_scratchpad">;
def int_npu_block_loadv8i32_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i32_masked">;
def int_npu_block_loadv8i32_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i32_scratchpad_masked">;

//Opcode: 10
//-v8 64-//
def int_npu_block_loadv8i64_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i64_scratchpad">;
def int_npu_block_loadv8f64_scratchpad : Intrinsic<[llvm_v8f64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8f64_scratchpad">;
def int_npu_block_loadv8i64_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i64_masked">;
def int_npu_block_loadv8f64_masked : Intrinsic<[llvm_v8f64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8f64_masked">;
def int_npu_block_loadv8i64_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8i64_scratchpad_masked">;
def int_npu_block_loadv8f64_scratchpad_masked : Intrinsic<[llvm_v8f64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8f64_scratchpad_masked">;

//Opcode: 11
//-v16 32-//
def int_npu_block_loadv16u8 : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u8">;
def int_npu_block_loadv16u8_scratchpad : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u8_scratchpad">;
def int_npu_block_loadv16u8_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u8_masked">;
def int_npu_block_loadv16u8_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u8_scratchpad_masked">;
//-v8 64-//
def int_npu_block_loadv8u8 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u8">;
def int_npu_block_loadv8u8_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u8_scratchpad">;
def int_npu_block_loadv8u8_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u8_masked">;
def int_npu_block_loadv8u8_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u8_scratchpad_masked">;

//Opcode: 12
//-v16 32-//
def int_npu_block_loadv16u16 : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u16">;
def int_npu_block_loadv16u16_scratchpad : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u16_scratchpad">;
def int_npu_block_loadv16u16_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u16_masked">;
def int_npu_block_loadv16u16_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv16u16_scratchpad_masked">;
//-v8 64-//
def int_npu_block_loadv8u16 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u16">;
def int_npu_block_loadv8u16_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u16_scratchpad">;
def int_npu_block_loadv8u16_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u16_masked">;
def int_npu_block_loadv8u16_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u16_scratchpad_masked">;

//Opcode: 13
//-v8 64-//
def int_npu_block_loadv8u32 : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u32">;
def int_npu_block_loadv8u32_scratchpad : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u32_scratchpad">;
def int_npu_block_loadv8u32_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u32_masked">;
def int_npu_block_loadv8u32_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [v16i32_ptr_ty],
	[IntrReadMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_loadv8u32_scratchpad_masked">;

//------ Vector load gather ------//

//-v16 32-//
def int_npu_gather_loadi32_scratchpad : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadi32_scratchpad">;
def int_npu_gather_loadf32_scratchpad : Intrinsic<[llvm_v16f32_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadf32_scratchpad">;
def int_npu_gather_loadi32_scratchpad_masked : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadi32_scratchpad_masked">;
def int_npu_gather_loadf32_scratchpad_masked : Intrinsic<[llvm_v16f32_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadf32_scratchpad_masked">;
//-v8 64-//
def int_npu_gather_loadi64_scratchpad : Intrinsic<[llvm_v8i64_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadi64_scratchpad">;
def int_npu_gather_loadf64_scratchpad : Intrinsic<[llvm_v8f64_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadf64_scratchpad">;
def int_npu_gather_loadi64_scratchpad_masked : Intrinsic<[llvm_v8i64_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadi64_scratchpad_masked">;
def int_npu_gather_loadf64_scratchpad_masked : Intrinsic<[llvm_v8f64_ty], [llvm_v16i32_ty],
	[IntrReadMem], "llvm.npu.__builtin_npu_gather_loadf64_scratchpad_masked">;

//------ Vector store block ------//

//Opcode: 36
//-v16 32-//
def int_npu_block_storev16i8 : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i8">;
def int_npu_block_storev16i8_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i8_scratchpad">;
def int_npu_block_storev16i8_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i8_masked">;
def int_npu_block_storev16i8_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i8_scratchpad_masked">;
//-v8 64-//
def int_npu_block_storev8i8 : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i8">;
def int_npu_block_storev8i8_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i8_scratchpad">;
def int_npu_block_storev8i8_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i8_masked">;
def int_npu_block_storev8i8_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i8_scratchpad_masked">;

//Opcode: 37
//-v16 32-//
def int_npu_block_storev16i16 : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i16">;
def int_npu_block_storev16i16_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i16_scratchpad">;
def int_npu_block_storev16i16_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i16_masked">;
def int_npu_block_storev16i16_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i16_scratchpad_masked">;
//-v8 64-//
def int_npu_block_storev8i16 : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i16">;
def int_npu_block_storev8i16_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i16_scratchpad">;
def int_npu_block_storev8i16_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i16_masked">;
def int_npu_block_storev8i16_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i16_scratchpad_masked">;


//Opcode: 38
//-v16 32-//
def int_npu_block_storev16i32_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i32_scratchpad">;
def int_npu_block_storev16f32_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v16f32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16f32_scratchpad">;
def int_npu_block_storev16i32_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i32_masked">;
def int_npu_block_storev16f32_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16f32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16f32_masked">;
def int_npu_block_storev16i32_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16i32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16i32_scratchpad_masked">;
def int_npu_block_storev16f32_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v16f32_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev16f32_scratchpad_masked">;
//-v8 64-//
def int_npu_block_storev8i32 : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i32">;
def int_npu_block_storev8i32_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i32_scratchpad">;
def int_npu_block_storev8i32_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i32_masked">;
def int_npu_block_storev8i32_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i32_scratchpad_masked">;


//Opcode: 39
//-v8 64-//
def int_npu_block_storev8i64_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i64_scratchpad">;
def int_npu_block_storev8f64_scratchpad : Intrinsic<[], [v16i32_ptr_ty, llvm_v8f64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8f64_scratchpad">;
def int_npu_block_storev8i64_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i64_masked">;
def int_npu_block_storev8f64_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8f64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8f64_masked">;
def int_npu_block_storev8i64_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8i64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8i64_scratchpad_masked">;
def int_npu_block_storev8f64_scratchpad_masked : Intrinsic<[], [v16i32_ptr_ty, llvm_v8f64_ty],
	[IntrWriteMem, IntrArgMemOnly], "llvm.npu.__builtin_npu_block_storev8f64_scratchpad_masked">;

// TODO: (Catello) add gather and scatter for v8i64. 
// The load gather must take a v16i32 containing the 8 addresses in the first 8 locations and returns the v8i64 variable.
// The store scatter must  take a v16i32 containing the 8 addresses in the first 8 locations where store the v8i64 variable.


//------ Vector store scatter ------//
def int_npu_scatter_storei32_scratchpad : Intrinsic<[], [llvm_v16i32_ty, llvm_v16i32_ty],
	[], "llvm.npu.__builtin_npu_scatter_storei32_scratchpad">;
def int_npu_scatter_storef32_scratchpad : Intrinsic<[], [llvm_v16i32_ty, llvm_v16f32_ty],
	[], "llvm.npu.__builtin_npu_scatter_storef32_scratchpad">;
def int_npu_scatter_storei32_scratchpad_masked : Intrinsic<[], [llvm_v16i32_ty, llvm_v16i32_ty],
	[], "llvm.npu.__builtin_npu_scatter_storei32_scratchpad_masked">;
def int_npu_scatter_storef32_scratchpad_masked : Intrinsic<[], [llvm_v16i32_ty, llvm_v16f32_ty],
	[], "llvm.npu.__builtin_npu_scatter_storef32_scratchpad_masked">;
	
def int_npu_scatter_storei64_scratchpad : Intrinsic<[], [llvm_v16i32_ty, llvm_v8i64_ty],
	[], "llvm.npu.__builtin_npu_scatter_storei64_scratchpad">;
def int_npu_scatter_storef64_scratchpad : Intrinsic<[], [llvm_v16i32_ty, llvm_v8f64_ty],
	[], "llvm.npu.__builtin_npu_scatter_storef64_scratchpad">;
def int_npu_scatter_storei64_scratchpad_masked : Intrinsic<[], [llvm_v16i32_ty, llvm_v8i64_ty],
	[], "llvm.npu.__builtin_npu_scatter_storei64_scratchpad_masked">;
def int_npu_scatter_storef64_scratchpad_masked : Intrinsic<[], [llvm_v16i32_ty, llvm_v8f64_ty],
	[], "llvm.npu.__builtin_npu_scatter_storef64_scratchpad_masked">;

// Vector Comparisions 32bit
def int_npu_mask_cmpi32_ugt : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_ugt">;
def int_npu_mask_cmpi32_uge : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_uge">;
def int_npu_mask_cmpi32_ult : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_ult">;
def int_npu_mask_cmpi32_ule : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_ule">;
def int_npu_mask_cmpi32_sgt : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_sgt">;
def int_npu_mask_cmpi32_sge : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_sge">;
def int_npu_mask_cmpi32_slt : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_slt">;
def int_npu_mask_cmpi32_sle : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_sle">;
def int_npu_mask_cmpi32_eq : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_eq">;
def int_npu_mask_cmpi32_ne : Intrinsic<[llvm_i32_ty], [llvm_v16i32_ty, llvm_v16i32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi32_ne">;
def int_npu_mask_cmpf32_gt : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_gt">;
def int_npu_mask_cmpf32_ge : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_ge">;
def int_npu_mask_cmpf32_le : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_le">;
def int_npu_mask_cmpf32_lt : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_lt">;
def int_npu_mask_cmpf32_eq : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_eq">;
def int_npu_mask_cmpf32_ne : Intrinsic<[llvm_i32_ty], [llvm_v16f32_ty, llvm_v16f32_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf32_ne">;

// Vector Comparisions 64bit
def int_npu_mask_cmpi64_ugt : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_ugt">;
def int_npu_mask_cmpi64_uge : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_uge">;
def int_npu_mask_cmpi64_ult : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_ult">;
def int_npu_mask_cmpi64_ule : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_ule">;
def int_npu_mask_cmpi64_sgt : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_sgt">;
def int_npu_mask_cmpi64_sge : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_sge">;
def int_npu_mask_cmpi64_slt : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_slt">;
def int_npu_mask_cmpi64_sle : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_sle">;
def int_npu_mask_cmpi64_eq : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_eq">;
def int_npu_mask_cmpi64_ne : Intrinsic<[llvm_i32_ty], [llvm_v8i64_ty, llvm_v8i64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpi64_ne">;
def int_npu_mask_cmpf64_gt : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_gt">;
def int_npu_mask_cmpf64_ge : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_ge">;
def int_npu_mask_cmpf64_le : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_le">;
def int_npu_mask_cmpf64_lt : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_lt">;
def int_npu_mask_cmpf64_eq : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_eq">;
def int_npu_mask_cmpf64_ne : Intrinsic<[llvm_i32_ty], [llvm_v8f64_ty, llvm_v8f64_ty],
	[IntrNoMem], "llvm.npu.__builtin_npu_mask_cmpf64_ne">;

//Barrier Format C
def int_npu_barrier : Intrinsic<[], [llvm_i32_ty, llvm_i32_ty], [],
	"llvm.npu.__builtin_npu_barrier">;
def int_npu_flush : Intrinsic<[], [llvm_i32_ty], [],
	"llvm.npu.__builtin_npu_flush">;
def int_npu_dcacheinv : Intrinsic<[], [llvm_i32_ty], [],
	"llvm.npu.__builtin_npu_dcacheinv">;

// Count Leading Zeroes
def int_npu_clzi32 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_clzi32">;
def int_npu_clzi64 : Intrinsic<[llvm_i64_ty], [llvm_i64_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_clzi64">;
def int_npu_clzv16i32 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_clzv16i32">;
def int_npu_clzv8i64 : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_clzv8i64">;

// Count Trailing Zeroes
def int_npu_ctzi32 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_ctzi32">;
def int_npu_ctzi64 : Intrinsic<[llvm_i64_ty], [llvm_i64_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_ctzi64">;
def int_npu_ctzv16i32 : Intrinsic<[llvm_v16i32_ty], [llvm_v16i32_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_ctzv16i32">;
def int_npu_ctzv8i64 : Intrinsic<[llvm_v8i64_ty], [llvm_v8i64_ty],
[IntrNoMem], "llvm.npu.__builtin_npu_ctzv8i64">;
}



