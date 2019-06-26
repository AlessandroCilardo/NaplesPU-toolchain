//===-- NaplesPUInstrInfo.td - Target Description for NaplesPU Target -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the NaplesPU instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "NaplesPUInstrFormats.td"

//===----------------------------------------------------------------------===//
// Instruction definition
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Arithmetic Instructions (Format R & I)
//===----------------------------------------------------------------------===//

//Arithmetic Integer Two operands
defm OR     : FArithInt_TwoOp<"or", or, 1>;
defm AND    : FArithInt_TwoOp<"and", and, 2>;
defm XOR    : FArithInt_TwoOp<"xor", xor, 3>;
defm ADD    : FArithInt_TwoOp<"add", add, 4>;
defm SUB    : FArithInt_TwoOp<"sub", sub, 5>;
defm MULL   : FArithInt_TwoOp<"mull", mul, 6>;
defm MULH   : FArithInt_TwoOp<"mulhs", mulhs, 7>;
defm MULHU  : FArithInt_TwoOp<"mulhu", mulhu, 8>;
defm SRA    : FSRInt_TwoOp<"ashr", sra, 9>;
defm SRL    : FSRInt_TwoOp<"shr", srl, 10>;
defm SLL    : FSRInt_TwoOp<"shl", shl, 11>;
//Arithmetic Integer One operand
defm CLZ    : FArithInt_OneOp<"clz", ctlz, 12>;
defm CTZ    : FArithInt_OneOp<"ctz", cttz, 13>;
//Compare Integer
defm SEQSI : FCompInt<"cmpeq", SETEQ, 14, int_npu_mask_cmpi32_eq>;//, int_npu_mask_cmpi64_eq>;
defm SNESI : FCompInt<"cmpne", SETNE, 15, int_npu_mask_cmpi32_ne>;//, int_npu_mask_cmpi64_ne>;
defm SGTSI : FCompInt<"cmpgt", SETGT, 16, int_npu_mask_cmpi32_sgt>;//, int_npu_mask_cmpi64_sgt>;
defm SGESI : FCompInt<"cmpge", SETGE, 17, int_npu_mask_cmpi32_sge>;//, int_npu_mask_cmpi64_sge>;
defm SLTSI : FCompInt<"cmplt", SETLT, 18, int_npu_mask_cmpi32_slt>;//, int_npu_mask_cmpi64_slt>;
defm SLESI : FCompInt<"cmple", SETLE, 19, int_npu_mask_cmpi32_sle>;//, int_npu_mask_cmpi64_sle>;
//Compare Unsigned
defm SGTUI : FCompInt<"cmpugt", SETUGT, 20, int_npu_mask_cmpi32_ugt>;//, int_npu_mask_cmpi64_ugt>;
defm SGEUI : FCompInt<"cmpuge", SETUGE, 21, int_npu_mask_cmpi32_uge>;//, int_npu_mask_cmpi64_uge>;
defm SLTUI : FCompInt<"cmpult", SETULT, 22, int_npu_mask_cmpi32_ult>;//, int_npu_mask_cmpi64_ult>;
defm SLEUI : FCompInt<"cmpule", SETULE, 23, int_npu_mask_cmpi32_ule>;//, int_npu_mask_cmpi64_ule>;

//Cross Product Instruction
def CROSSPROD_32 : FR_TwoOp_Unmasked_32 <
  (outs VR512W:$dst),
  (ins VR512W:$src0, VR512W:$src1),
  "crp $dst, $src0, $src1",
  [(set v16i32:$dst, (int_npu_crossprodv16i32 v16i32:$src0, v16i32:$src1))],
  63,
  Fmt_V,
  Fmt_V,
  Fmt_V>;

//Shuffle
def SHUFFLEI_32 : FR_TwoOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins VR512W:$src0, VR512W:$src1),
  "shuffle_i32 $dst, $src0, $src1",
  [(set v16i32:$dst, (int_npu_shufflei32 v16i32:$src0, v16i32:$src1))],
  24,
  Fmt_V,
  Fmt_V,
  Fmt_V>;
/*
def SHUFFLEI_64 : FR_TwoOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins VR512L:$src0, VR512L:$src1),
  "shuffle_i64 $dst, $src0, $src1",
  [(set v8i64:$dst, (int_npu_shufflei64 v8i64:$src0, v8i64:$src1))],
  24,
  Fmt_V,
  Fmt_V,
  Fmt_V>;
*/
//Get lane
def GET_LANEI_32 : FR_TwoOp_Unmasked_32<
  (outs GPR32:$dst),
  (ins VR512W:$src0, GPR32:$src1),
  "getlane_i32 $dst, $src0, $src1",
  [(set i32:$dst, (extractelt v16i32:$src0, i32:$src1))],
  25,
  Fmt_S,
  Fmt_V,
  Fmt_S>;

def GET_LANEIimm : FI_OneOp_Unmasked<
  (outs GPR32:$dst),
  (ins VR512W:$src, SIMM9OP:$imm),
  "getlanei $dst, $src, $imm",
  [(set i32:$dst, (extractelt v16i32:$src, simm9:$imm))],
  25,
  Fmt_S,
  Fmt_V>;
/*
def GET_LANEI_64 : FR_TwoOp_Unmasked_64<
  (outs GPR64:$dst),
  (ins VR512L:$src0, GPR32:$src1),
  "getlane_i64 $dst, $src0, $src1",
  [(set i64:$dst, (extractelt v8i64:$src0, i32:$src1))],
  25,
  Fmt_S,
  Fmt_V,
  Fmt_S>;
*/
//Move register - register
def MOVE_SS_32 : FR_OneOp_Unmasked_32<
  (outs GPR32:$dst),
  (ins GPR32:$src0),
  "move_i32 $dst, $src0",
  [],
  32,
  Fmt_S,
  Fmt_S>;
/*
def MOVE_SS_64 : FR_OneOp_Unmasked_64<
  (outs GPR64:$dst),
  (ins GPR64:$src0),
  "move_i64 $dst, $src0",
  [],
  32,
  Fmt_S,
  Fmt_S>;
*/
def MOVE_VS_32 : FR_OneOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins GPR32:$src0),
  "move_i32 $dst, $src0",
  [(set v16i32:$dst, (splat i32:$src0))],
  32,
  Fmt_V,
  Fmt_S>;
/*
def MOVE_VS_64 : FR_OneOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins GPR64:$src0),
  "move_i64 $dst, $src0",
  [(set v8i64:$dst, (splat i64:$src0))],
  32,
  Fmt_V,
  Fmt_S>;
  */
def MOVE_VV_32 : FR_OneOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins VR512W:$src0),
  "move_i32 $dst, $src0",
  [],
  32,
  Fmt_V,
  Fmt_V>;
/*
def MOVE_VV_64 : FR_OneOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins VR512L:$src0),
  "move_i64 $dst, $src0",
  [],
  32,
  Fmt_V,
  Fmt_V>;
*/
let Constraints = "$dst = $oldvalue", isAsmParserOnly=1 in {

  def MOVE_VS_M_32 : FR_OneOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins GPR32:$src0, VR512W:$oldvalue),
    "move_i32 $dst, $src0",
    [(set v16i32:$dst, (int_npu_vector_mixi32 (splat i32:$src0), v16i32:$oldvalue))],
    32,
    Fmt_V,
    Fmt_S>;
/*
  def MOVE_VS_M_64 : FR_OneOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins GPR64:$src0, VR512L:$oldvalue),
    "move_i64 $dst, $src0",
    [(set v8i64:$dst, (int_npu_vector_mixi64 (splat i64:$src0), v8i64:$oldvalue))],
    32,
    Fmt_V,
    Fmt_S>;
*/
  def MOVE_VV_M_32 : FR_OneOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$oldvalue),
    "move_i32 $dst, $src0",
    [(set v16i32:$dst, (int_npu_vector_mixi32 v16i32:$src0, v16i32:$oldvalue))],
    32,
    Fmt_V,
    Fmt_V>;
/*
  def MOVE_VV_M_64 : FR_OneOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$oldvalue),
    "move_i64 $dst, $src0",
    [(set v8i64:$dst, (int_npu_vector_mixi64 v8i64:$src0, v8i64:$oldvalue))],
    32,
    Fmt_V,
    Fmt_V>;*/
}

//Arithmetic Float Two operands
defm ADDF   : FArithFloat_TwoOp<"fadd", fadd, 33>;
defm SUBF   : FArithFloat_TwoOp<"fsub", fsub, 34>;
defm MULF   : FArithFloat_TwoOp<"fmul", fmul, 35>;
defm DIVF   : FArithFloat_TwoOp<"fdiv", fdiv, 36>;
//Compare Float
def FloatCompareType: SDTypeProfile<1, 2, [
  SDTCisInt<0>, SDTCisFP<1>, SDTCisSameAs<1, 2>
]>;
def FGT: SDNode<"NaplesPUISD::FGT", FloatCompareType>;
def FGE: SDNode<"NaplesPUISD::FGE", FloatCompareType>;
def FLT: SDNode<"NaplesPUISD::FLT", FloatCompareType>;
def FLE: SDNode<"NaplesPUISD::FLE", FloatCompareType>;
def FEQ: SDNode<"NaplesPUISD::FEQ", FloatCompareType>;
def FNE: SDNode<"NaplesPUISD::FNE", FloatCompareType>;

defm SEQFO : FCompFloat<"cmpfeq", FEQ, 37, int_npu_mask_cmpf32_eq>;//, int_npu_mask_cmpf64_eq>;
defm SNEFO : FCompFloat<"cmpfne", FNE, 38, int_npu_mask_cmpf32_ne>;//, int_npu_mask_cmpf64_ne>;
defm SGTFO : FCompFloat<"cmpfgt", FGT, 39, int_npu_mask_cmpf32_gt>;//, int_npu_mask_cmpf64_gt>;
defm SGEFO : FCompFloat<"cmpfge", FGE, 40, int_npu_mask_cmpf32_ge>;//, int_npu_mask_cmpf64_ge>;
defm SLTFO : FCompFloat<"cmpflt", FLT, 41, int_npu_mask_cmpf32_lt>;//, int_npu_mask_cmpf64_lt>;
defm SLEFO : FCompFloat<"cmpfle", FLE, 42, int_npu_mask_cmpf32_le>;//, int_npu_mask_cmpf64_le>;

//sign extension
defm SEXT8_32 : FSext_32<"sext8", i8, 43, v16i8>;
//defm SEXT8_64 : FSext_64<"sext8", i8, 43, v8i8>;
defm SEXT16_32 : FSext_32<"sext16", i16, 44, v16i16>;
//defm SEXT16_64 : FSext_64<"sext16", i16, 44, v8i16>;
//defm SEXT32_64 : FSext_64<"sext32", i32, 45, v8i32>;
/*
// Float to Double conversion
def FTOD_SS : FR_OneOp_Unmasked_64<
  (outs GPR64:$dst),
  (ins GPR32:$src0),
  "f32tof64 $dst, $src0",
  [(set f64:$dst, (fextend f32:$src0))],
  46,
  Fmt_S,
  Fmt_S>;

//TODO: we should decide how to handle v16f32 -> v8f64. This may not work. Intrinsic? MAybe we should just convert v8f32->v8f64
def FTOD_VV : FR_OneOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins VR512W:$src0),
  "f32tof64 $dst, $src0",
  [(set v8f64:$dst, (int_npu_v16f32tov8f64 v16f32:$src0))],
  46,
  Fmt_V,
  Fmt_V>;
  
let Constraints = "$dst = $oldvalue" in {
  def FTOD_VV_M : FR_OneOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512W:$src0, VR512L:$oldvalue),
    "f32tof64.m $dst, $src0",
    [(set v8f64:$dst, (int_npu_vector_mixf64 (int_npu_v16f32tov8f64 v16f32:$src0), v8f64:$oldvalue))],
    46,
    Fmt_V,
    Fmt_V>;
}

// Double to Float conversion
def DTOF_SS : FR_OneOp_Unmasked_32<
  (outs GPR32:$dst),
  (ins GPR64:$src0),
  "f64tof32 $dst, $src0",
  [(set f32:$dst, (fround f64:$src0))],
  47,
  Fmt_S,
  Fmt_S>;
  
def DTOF_VV : FR_OneOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins VR512L:$src0),
  "f64tof32 $dst, $src0",
  [(set v16f32:$dst, (int_npu_v8f64tov16f32 v8f64:$src0))],
  47,
  Fmt_V,
  Fmt_V>;

let Constraints = "$dst = $oldvalue" in {
  def DTOF_VV_M : FR_OneOp_Masked_32<
    (outs VR512W:$dst),
    (ins VR512L:$src0, VR512W:$oldvalue),
    "f64tof32.m $dst, $src0",
    [(set v16f32:$dst, (int_npu_vector_mixf32 (int_npu_v8f64tov16f32 v8f64:$src0), v16f32:$oldvalue))],
    47,
    Fmt_V,
    Fmt_V>;
}
*/
//Integer to Float conversion
def I32TOF32_SS : FR_OneOp_Unmasked_32<
  (outs GPR32:$dst),
  (ins GPR32:$src0),
  "itof_i32 $dst, $src0",
  [(set f32:$dst, (sint_to_fp i32:$src0))],
  48,
  Fmt_S,
  Fmt_S>;
/*
def I64TOF64_SS : FR_OneOp_Unmasked_64<
  (outs GPR64:$dst),
  (ins GPR64:$src0),
  "itof_i64 $dst, $src0",
  [(set f64:$dst, (sint_to_fp i64:$src0))],
  48,
  Fmt_S,
  Fmt_S>;
  */
def I32TOF32_VV_U : FR_OneOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins VR512W:$src0),
  "itof_i32 $dst, $src0",
  [(set v16f32:$dst, (sint_to_fp v16i32:$src0))],
  48,
  Fmt_V,
  Fmt_V>;
  /*
def I64TOF64_VV_U : FR_OneOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins VR512L:$src0),
  "itof_i64 $dst, $src0",
  [(set v8f64:$dst, (sint_to_fp v8i64:$src0))],
  48,
  Fmt_V,
  Fmt_V>;
*/
let Constraints = "$dst = $oldvalue" in {
  def I32TOF32_VV_M : FR_OneOp_Masked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$oldvalue),
    "itof_i32.m $dst, $src0",
    [(set v16f32:$dst, (int_npu_vector_mixf32 (sint_to_fp v16i32:$src0), v16f32:$oldvalue))],
    48,
    Fmt_V,
    Fmt_V>;
/*	
  def I64TOF64_VV_M : FR_OneOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$oldvalue),
    "itof_i64.m $dst, $src0",
    [(set v8f64:$dst, (int_npu_vector_mixf64 (sint_to_fp v8i64:$src0), v8f64:$oldvalue))],
    48,
    Fmt_V,
    Fmt_V>;*/
}

//Float to Integer conversion
def F32TOI32_SS : FR_OneOp_Unmasked_32<
  (outs GPR32:$dst),
  (ins GPR32:$src0),
  "ftoi_i32 $dst, $src0",
  [(set i32:$dst, (fp_to_sint f32:$src0))],
  49,
  Fmt_S,
  Fmt_S>;
  /*
def F64TOI64_SS : FR_OneOp_Unmasked_64<
  (outs GPR64:$dst),
  (ins GPR64:$src0),
  "ftoi_i64 $dst, $src0",
  [(set i64:$dst, (fp_to_sint f64:$src0))],
  49,
  Fmt_S,
  Fmt_S>;
*/
def F32TOI32_VV_U : FR_OneOp_Unmasked_32<
  (outs VR512W:$dst),
  (ins VR512W:$src0),
  "ftoi_i32 $dst, $src0",
  [(set v16i32:$dst, (fp_to_sint v16f32:$src0))],
  49,
  Fmt_V,
  Fmt_V>;
  /*
def F64TOI64_VV_U : FR_OneOp_Unmasked_64<
  (outs VR512L:$dst),
  (ins VR512L:$src0),
  "ftoi_i64 $dst, $src0",
  [(set v8i64:$dst, (fp_to_sint v8f64:$src0))],
  49,
  Fmt_V,
  Fmt_V>;
*/
let Constraints = "$dst = $oldvalue" in {
  def F32TOI32_VV_M : FR_OneOp_Masked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$oldvalue),
    "ftoi_i32.m $dst, $src0",
    [(set v16i32:$dst, (int_npu_vector_mixi32 (fp_to_sint v16f32:$src0), v16i32:$oldvalue))],
    49,
    Fmt_V,
    Fmt_V>;
/*	
  def F64TOI64_VV_M : FR_OneOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$oldvalue),
    "ftoi_i64.m $dst, $src0",
    [(set v8i64:$dst, (int_npu_vector_mixi64 (fp_to_sint v8f64:$src0), v8i64:$oldvalue))],
    49,
    Fmt_V,
    Fmt_V>;*/
}

//===----------------------------------------------------------------------===//
// MOVEI Instructions (Format MOVEI)
//===----------------------------------------------------------------------===//

defm MOVEIL : FMOVEI_ALL<"moveil", 0>;
defm MOVEIH : FMOVEI_ALL<"moveih", 1>;
defm MOVEI : FMOVEI_ALL<"movei", 2>;


//===----------------------------------------------------------------------===//
// LOAD/STORE SPECIAL REGISTERS
//===----------------------------------------------------------------------===//

def READMR  : READ_SPR<MaskReg, "read_mr", int_npu_read_mask_reg>;
def WRITEMR : WRITE_SPR<MaskReg, "write_mr", int_npu_write_mask_reg>;

//Create Mask Instruction
def CREATEMASK_32 : FR_OneOp_Unmasked_32 <
  (outs GPR32:$dst),
  (ins VR512W:$src0),
  "crt_mask_i32 $dst, $src0",
  [(set i32:$dst, (int_npu_createmaskv16i32 v16i32:$src0))],
  50,
  Fmt_S,
  Fmt_V>;
/*
def CREATEMASK_64 : FR_OneOp_Unmasked_64 <
  (outs GPR32:$dst),
  (ins VR512L:$src0),
  "crt_mask_i64 $dst, $src0",
  [(set i32:$dst, (int_npu_createmaskv8i64 v8i64:$src0))],
  50,
  Fmt_S,
  Fmt_V>;
*/

//===----------------------------------------------------------------------===//
// MEM Instructions (Format M)
//===----------------------------------------------------------------------===//

// Scalar load to 32-bit registers
defm L32BS : FMLoadScalar_32<"_s8", sextloadi8_mem, sextloadi8_scratch, 0>;
defm L32SS : FMLoadScalar_32<"_s16", sextloadi16_mem, sextloadi16_scratch, 1>;
defm L32W  : FMLoadScalar_32<"", MemLoad, ScratchpadLoad, 2>;
defm L32BU : FMLoadScalar_32<"_u8", zextloadi8_mem, zextloadi8_scratch, 4>;
defm L32SU : FMLoadScalar_32<"_u16", zextloadi16_mem, zextloadi16_scratch, 5>;
// Scalar load to 64-bit registers
/*defm L64BS : FMLoadScalar_64<"_s8", sextloadi8_mem, sextloadi8_scratch, 0>;
defm L64SS : FMLoadScalar_64<"_s16", sextloadi16_mem, sextloadi16_scratch, 1>;
defm L64WS : FMLoadScalar_64<"_s32", sextloadi32_mem, sextloadi32_scratch, 2>;
defm L64L  : FMLoadScalar_64<"", MemLoad, ScratchpadLoad, 3>;
defm L64BU : FMLoadScalar_64<"_u8", zextloadi8_mem, zextloadi8_scratch, 4>;
defm L64SU : FMLoadScalar_64<"_u16", zextloadi16_mem, zextloadi16_scratch, 5>;
defm L64WU : FMLoadScalar_64<"_u32", zextloadi32_mem, zextloadi32_scratch, 6>;
*/// Scalar store from 32-bit registers
defm S32B : FMStoreScalar_32<"_8", truncstorei8_mem, truncstorei8_scratch, 32>;
defm S32S : FMStoreScalar_32<"_16", truncstorei16_mem, truncstorei16_scratch, 33>;
defm S32W : FMStoreScalar_32<"", MemStore, ScratchpadStore, 34>;
// Scalar store from 64-bit registers
/*defm S64B : FMStoreScalar_64<"_8", truncstorei8_mem, truncstorei8_scratch, 32>;
defm S64S : FMStoreScalar_64<"_16", truncstorei16_mem, truncstorei16_scratch, 33>;
defm S64W : FMStoreScalar_64<"_32", truncstorei32_mem, truncstorei32_scratch, 34>;
defm S64L : FMStoreScalar_64<"", MemStore, ScratchpadStore, 35>;
*/
//--------------------------------------------------------------------------------//
//Vector load block 
//--------------------------------------------------------------------------------//

// Vector load to 32-bit registers
defm LV32B : FMLoadVector_32<"_v16i8", sextloadv16i8_mem, sextloadv16i8_scratch, 
                              int_npu_block_loadv16i8_masked, int_npu_block_loadv16i8_scratchpad_masked, 
                              7>;
defm LV32S : FMLoadVector_32<"_v16i16", sextloadv16i16_mem, sextloadv16i16_scratch, 
                              int_npu_block_loadv16i16_masked, int_npu_block_loadv16i16_scratchpad_masked, 
                              8>;
defm LV32W : FMLoadVector_32<"_v16i32", MemLoad, ScratchpadLoad, 
                              int_npu_block_loadv16i32_masked, int_npu_block_loadv16i32_scratchpad_masked, 
                              9>;

defm LV32BU : FMLoadVector_32<"_v16u8", zextloadv16i8_mem, zextloadv16i8_scratch, 
                              int_npu_block_loadv16u8_masked, int_npu_block_loadv16u8_scratchpad_masked, 
                              11>;
defm LV32SU : FMLoadVector_32<"_v16u16", zextloadv16i16_mem, zextloadv16i16_scratch, 
                              int_npu_block_loadv16u16_masked, int_npu_block_loadv16u16_scratchpad_masked, 
                              12>;

// Vector load to 64-bit registers
/*defm LV64B : FMLoadVector_64<"_v8i8", sextloadv8i8_mem, sextloadv8i8_scratch, 
                              int_npu_block_loadv8i8_masked, int_npu_block_loadv8i8_scratchpad_masked, 
                              7>;
defm LV64S : FMLoadVector_64<"_v8i16", sextloadv8i16_mem, sextloadv8i16_scratch, 
                              int_npu_block_loadv8i16_masked, int_npu_block_loadv8i16_scratchpad_masked, 
                              8>;
defm LV64W : FMLoadVector_64<"_v8i32", sextloadv8i32_mem, sextloadv8i32_scratch, 
                              int_npu_block_loadv8i32_masked, int_npu_block_loadv8i32_scratchpad_masked, 
                              9>;
defm LV64L : FMLoadVector_64<"_v8i64", MemLoad, ScratchpadLoad, 
                              int_npu_block_loadv8i64_masked, int_npu_block_loadv8i64_scratchpad_masked, 
                              10>;

defm LV64BU : FMLoadVector_64<"_v8u8", zextloadv8i8_mem, zextloadv8i8_scratch, 
                              int_npu_block_loadv8u8_masked, int_npu_block_loadv8u8_scratchpad_masked, 
                              11>;
defm LV64SU : FMLoadVector_64<"_v8u16", zextloadv8i16_mem, zextloadv8i16_scratch, 
                              int_npu_block_loadv8u16_masked, int_npu_block_loadv8u16_scratchpad_masked, 
                              12>;
defm LV64WU : FMLoadVector_64<"_v8u32", zextloadv8i32_mem, zextloadv8i32_scratch, 
                              int_npu_block_loadv8u32_masked, int_npu_block_loadv8u32_scratchpad_masked, 
                              13>;
  */
//--------------------------------------------------------------------------------//
//Vector load gather 
//--------------------------------------------------------------------------------//

// Vector gather to 32-bit registers
defm GLV32W : FMGather_32<"32", int_npu_gather_loadi32_scratchpad,
                          int_npu_gather_loadi32_scratchpad_masked,
                          16>;
    /*                      
// Vector gather to 64-bit registers
defm GLV64L : FMGather_64<"64", int_npu_gather_loadi64_scratchpad,
                          int_npu_gather_loadi64_scratchpad_masked,
                          17>;
*/
//--------------------------------------------------------------------------------//
//Vector store block 
//--------------------------------------------------------------------------------//

// Vector store from 32-bit registers
defm SV32B : FMStoreVector_32<"_v16i8", truncstorev16i8_mem, truncstorev16i8_scratch, 
                              int_npu_block_storev16i8_masked, int_npu_block_storev16i8_scratchpad_masked, 
                              36>;
defm SV32S : FMStoreVector_32<"_v16i16", truncstorev16i16_mem, truncstorev16i16_scratch, 
                              int_npu_block_storev16i16_masked, int_npu_block_storev16i16_scratchpad_masked, 
                              37>;
defm SV32W : FMStoreVector_32<"_v16i32", MemStore, ScratchpadStore, 
                              int_npu_block_storev16i32_masked, int_npu_block_storev16i32_scratchpad_masked, 
                              38>;
/*
// Vector store from 64-bit registers
defm SV64B : FMStoreVector_64<"_v8i8", truncstorev8i8_mem, truncstorev8i8_scratch, 
                              int_npu_block_storev8i8_masked, int_npu_block_storev8i8_scratchpad_masked, 
                              36>;
defm SV64S : FMStoreVector_64<"_v8i16", truncstorev8i16_mem, truncstorev8i16_scratch, 
                              int_npu_block_storev8i16_masked, int_npu_block_storev8i16_scratchpad_masked, 
                              37>;
defm SV64W : FMStoreVector_64<"_v8i32", truncstorev8i32_mem, truncstorev8i32_scratch, 
                              int_npu_block_storev8i32_masked, int_npu_block_storev8i32_scratchpad_masked, 
                              38>;
defm SV64L : FMStoreVector_64<"_v8i64", MemStore, ScratchpadStore, 
                              int_npu_block_storev8i64_masked, int_npu_block_storev8i64_scratchpad_masked, 
                              39>;
*/
//--------------------------------------------------------------------------------//
//Vector store scatter 
//--------------------------------------------------------------------------------//

// Vector scatter from 32-bit registers
defm SSV32W : FMScatter_32<"32",int_npu_scatter_storei32_scratchpad, 
                              int_npu_scatter_storei32_scratchpad_masked, 
                              42>;
/*
// Vector scatter from 64-bit registers
defm SSV64L : FMScatter_64<"64",int_npu_scatter_storei64_scratchpad, 
                              int_npu_scatter_storei64_scratchpad_masked, 
                              43>;
*/
//===----------------------------------------------------------------------===//
// Jump and Branch Instructions (Format J/JR)
//===----------------------------------------------------------------------===//

let isTerminator = 1 in {
  //Unconditional jump with offset, PC:=PC+addr
  def JMP : FJ<
    (outs),
    (ins brtarget:$addr),
    "jmp $addr",
    [(br bb:$addr)],
    0>
  {
    let isBarrier = 1;
  }
  
  //Unconditional jump with register PC:=DST
  def JUMPREG : FJR<
  (outs),
  (ins GPR32:$cond),
  "jmp $cond",
  [(brind i32:$cond)],
  0>
  {
    let isBranch = 1;
    let isIndirectBranch = 1;
    let isBarrier = 1;
  }

  // Return PC:=RA
  def JRET : FJR<
  (outs),
  (ins),
  "jret",
  [(return)],
  3>
  {
    let Inst{23-18} = 0;
    let Inst{17-0} = 0;

    let isReturn = 1;
    let isTerminator = 1;
    let isBarrier = 1;
  }


  /*
  // JMP with register PC:=DST, implemented with MOVE
  def JUMPREG : FR_OneOp_Unmasked_32<
  (outs),
  (ins GPR32:$dst),
  "jmp $dst",
  [(brind i32:$dst)],
  32,
  Fmt_S,
  Fmt_S>
  {
    bits<6> dst;

	let Inst{29-24} = 32; // opcode: move
    let Inst{23-18} = 63; // PC
	let Inst{17-12} = dst;
	let Inst{11-6} = 0;

    let isBranch = 1;
    let isIndirectBranch = 1;
    let isBarrier = 1;
  }

  // Return PC:=RA, implemented with MOVE
  def JRET : FR_OneOp_Unmasked_32<
  (outs),
  (ins),
  "jret",
  [(return)],
  32,
  Fmt_S,
  Fmt_S>
  {
    let Inst{29-24} = 32; // opcode: move
    let Inst{23-18} = 63; // PC
    let Inst{17-12} = 62; // RA
    let Inst{11-6} = 0;

    let isReturn = 1;
    let isTerminator = 1;
    let isBarrier = 1;
  }
  */

  //Conditional Branch
  def BEQZ : FJR<
    (outs),
    (ins GPR32:$cond, brtarget:$addr),
    "beqz $cond, $addr",
    [],
    5>;

  //Conditional Branch
  def BNEZ : FJR<
    (outs),
    (ins GPR32:$cond, brtarget:$addr),
    "bnez $cond, $addr",
    [],
    6>;
}

let isCall = 1, Defs = [ RA_REG ] in {
  //Jump with offset and save return address RA:=PC+4, PC:=PC+addr
  def JMPSR_OFF : FJ<
    (outs),
    (ins calltarget:$addr, variable_ops),
    "jmpsr $addr",
    [],
    1>;

  //Jump with register and save return address RA:=PC+4, PC:=DST
  def JMPSR_REG : FJR<
    (outs),
    (ins GPR32:$dst, variable_ops),
    "jmpsr $dst",
    [(call i32:$dst)],
    1>
  {
    bits<6> dst;

    let Inst{23-18} = dst;
  }
}
 
//===----------------------------------------------------------------------===//
// Control Instructions (Format C)
//===----------------------------------------------------------------------===//

def BARRIER : FC<
  (outs),
  (ins GPR32:$src0, GPR32:$src1),
  "barrier $src0, $src1",
  [(int_npu_barrier i32:$src0, i32:$src1)],
  0>;
  
def FLUSH : FC<
  (outs),
  (ins GPR32:$src0),
  "flush $src0",
  [(int_npu_flush i32:$src0)],
  2>{
    let Inst{17-12} = 0;
  }

// READ Control Register
def READCR : FC<
  (outs GPR32:$src0),
  (ins GPR32:$src1),
  "read_cr $src0, $src1",
  [(set i32:$src0, (int_npu_read_control_reg i32:$src1))],
  3>;

// WRITE Control Register
def WRITECR : FC<
  (outs),
  (ins GPR32:$src0, GPR32:$src1),
  "write_cr $src0, $src1",
  [(int_npu_write_control_reg i32:$src0, i32:$src1)],
  4>;


//===----------------------------------------------------------------------===//
// Miscellaneous Instructions
//===----------------------------------------------------------------------===//

def NOP : InstNaplesPU<
  (outs),
  (ins),
  "nop",
  []>
{
  let Inst{31-0} = 0xffffffff;
}

def LOAD_EFFECTIVE_ADDR : InstNaplesPU<
  (outs GPR32:$dst),
  (ins LEAri:$addr),
  "_lea $dst, $addr",
  [(set i32:$dst, ADDRri:$addr)]> {

  let isAsmParserOnly = 1;   // Don't disassemble

  bits<16> addr;
  bits<6> dst;

  let Inst{31-29} = 0b010;
  let Inst{28-24} = 4;
  let Inst{23-18} = dst;
  let Inst{17-2} = addr;
  let Inst{1} = 0;
  let Inst{0} = 0;

}

//TODO: (Catello) Add symbol in NaplesPUAsmParser::ProcessInstruction.
// This node defines the lea pseudo-instruction that can be used in
// assembly files.
def LOAD_EFFECTIVE_ADDR_SYM : InstNaplesPU<
  (outs GPR32:$dst),
  (ins symref:$label),
  "lea $dst, $label",
  []> {

  let isPseudo = 1;

}

def LEAH : InstNaplesPU<
  (outs GPR32:$dst),
  (ins ABSh:$addr),
  "leah $dst, $addr",
  []> {
  let isAsmParserOnly = 1;   // Don't disassemble

  bits<16> addr;
  bits<6> dst;

  let Inst{31-27} = 0b01100; // Format MOVEI
  let Inst{26-24} = 1; //Opcode MOVEI
  let Inst{23-18} = dst;
  let Inst{17-2} = addr;
  let Inst{1} = 0;
  let Inst{0} = 0;
}


def LEAL : InstNaplesPU<
  (outs GPR32:$dst),
  (ins GPR32:$addrh, ABSl:$addr),
  "leal $dst, $addr",
  []> {
  let isAsmParserOnly = 1;   // Don't disassemble
  
  //costraint used to let llvm use the same destination register as leah
  let Constraints = "$dst = $addrh"; 

  bits<16> addr;
  bits<6> dst;

  let Inst{31-27} = 0b01100; // Format MOVEI
  let Inst{26-24} = 0; //Opcode MOVEI
  let Inst{23-18} = dst;
  let Inst{17-2} = addr;
  let Inst{1} = 0;
  let Inst{0} = 0;
}



//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

// These pseudo ops capture outgoing argument space on the stack and will be removed
// by later passes.
let Defs = [ SP_REG ], Uses = [ SP_REG ], hasSideEffects = 1 in {
   def ADJCALLSTACKDOWN : Pseudo<
     (outs),
     (ins i32imm:$amt1, i32imm:$amt2),
     [(callseq_start timm:$amt1, timm:$amt2)]>;
 
   def ADJCALLSTACKUP : Pseudo<
     (outs),
     (ins i32imm:$amt1, i32imm:$amt2),
     [(callseq_end timm:$amt1, timm:$amt2)]>;
 }

// SELECT pseudo instructions. This architecture doesn't actually have a scalar
// conditional move instruction. These will be replaced in a later pass
// with a diamond pattern of conditional branches.
//
let usesCustomInserter = 1 in {
  def SELECTI : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$pred, GPR32:$true, GPR32:$false),
    [(set i32:$dst, (selcondresult i32:$pred, i32:$true, i32:$false))]>;
/*
  def SELECTLI : Pseudo<
    (outs GPR64:$dst),
    (ins GPR32:$pred, GPR64:$true, GPR64:$false),
    [(set i64:$dst, (selcondresult i32:$pred, i64:$true, i64:$false))]>;
*/
  def SELECTF : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$pred, GPR32:$true, GPR32:$false),
    [(set f32:$dst, (selcondresult i32:$pred, f32:$true, f32:$false))]>;
/*
  def SELECTD : Pseudo<
    (outs GPR64:$dst),
    (ins GPR32:$pred, GPR64:$true, GPR64:$false),
    [(set f64:$dst, (selcondresult i32:$pred, f64:$true, f64:$false))]>;
*/
  def SELECTVI : Pseudo<
    (outs VR512W:$dst),
    (ins GPR32:$pred, VR512W:$true, VR512W:$false),
    [(set v16i32:$dst, (selcondresult i32:$pred, v16i32:$true, v16i32:$false))]>;
/*
  def SELECTVLI : Pseudo<
    (outs VR512L:$dst),
    (ins GPR32:$pred, VR512L:$true, VR512L:$false),
    [(set v8i64:$dst, (selcondresult i32:$pred, v8i64:$true, v8i64:$false))]>;
*/
  def SELECTVF : Pseudo<
    (outs VR512W:$dst),
    (ins GPR32:$pred, VR512W:$true, VR512W:$false),
    [(set v16f32:$dst, (selcondresult i32:$pred, v16f32:$true, v16f32:$false))]>;
/*
  def SELECTVD : Pseudo<
    (outs VR512L:$dst),
    (ins GPR32:$pred, VR512L:$true, VR512L:$false),
    [(set v8f64:$dst, (selcondresult i32:$pred, v8f64:$true, v8f64:$false))]>;
*/}

let usesCustomInserter = 1 in {
  def LoadI32 : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$val),
    [(set i32:$dst, (i32 imm:$val))]>;
/*
  def LoadI64 : Pseudo< 
    (outs GPR64:$dst),
    (ins GPR64:$val),
    []>;//[(set i64:$dst, (i64 imm:$val))]>;*/

  def LoadF32 : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$val),
    [(set f32:$dst, (f32 fpimm:$val))]>;
/*
  def LoadF64 : Pseudo<
    (outs GPR64:$dst),
    (ins GPR64:$val),
    []>;//[(set f64:$dst, (f64 fpimm:$val))]>;*/
}

let usesCustomInserter = 1 in {
  def InsertELT32 : Pseudo<
    (outs VR512W:$dst),
    (ins VR512W:$vec, GPR32:$elem, GPR32:$pos),
    [(set v16i32:$dst, (insert_elt v16i32:$vec, i32:$elem, i32:$pos))]>;

/*  def InsertELT64 : Pseudo<
    (outs VR512L:$dst),
    (ins VR512L:$vec, GPR64:$elem, GPR32:$pos),
    [(set v8i64:$dst, (insert_elt v8i64:$vec, i64:$elem, i32:$pos))]>;
*/}

def : Pat <(v16f32 (insert_elt v16f32:$vec, f32:$elem, i32:$pos)), (v16f32 (InsertELT32 v16f32:$vec, f32:$elem, i32:$pos))>;
/*def : Pat <(v8f64 (insert_elt v8f64:$vec, f64:$elem, i32:$pos)), (v8f64 (InsertELT64 v8f64:$vec, f64:$elem, i32:$pos))>;
*/
// Atomics
// TODO:(Catello) modify atomics in order to suit hardware instructions
let usesCustomInserter = 1 in {
 defm ATOMIC_LOAD_ADD : AtomicBinary<atomic_load_add>;
 defm ATOMIC_LOAD_SUB : AtomicBinary<atomic_load_sub>;
 defm ATOMIC_LOAD_AND : AtomicBinary<atomic_load_and>;
 defm ATOMIC_LOAD_OR  : AtomicBinary<atomic_load_or>;
 defm ATOMIC_LOAD_XOR : AtomicBinary<atomic_load_xor>;
 defm ATOMIC_LOAD_NAND : AtomicBinary<atomic_load_nand>;

def ATOMIC_CMP_SWAP : Pseudo<
   (outs GPR32:$dst),
   (ins GPR32:$ptr, GPR32:$cmp, GPR32:$swap),
   [(set i32:$dst, (atomic_cmp_swap GPR32:$ptr, GPR32:$cmp, GPR32:$swap))]>;

 def ATOMIC_SWAP : Pseudo<
   (outs GPR32:$dst),
  (ins GPR32:$ptr, GPR32:$swap),
   [(set i32:$dst, (atomic_swap GPR32:$ptr, GPR32:$swap))]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

// 64-bit immediate insertion pattern. It breaks the immediate value in two 32-bit 
// parts and load them in the apporpriate sub-register.
/*def : Pat<(i64 imm:$val),
       (INSERT_SUBREG (INSERT_SUBREG (i64 (IMPLICIT_DEF)),
                      (LoadI32 (i32 (HI32I $val))),
                      sub_odd),
      (LoadI32 (i32 (LO32I $val))),
      sub_even)>;
    
def : Pat<(f64 fpimm:$val),
       (INSERT_SUBREG (INSERT_SUBREG (f64 (IMPLICIT_DEF)),
                      (LoadI32 (i32 (HI32F $val))),
                      sub_odd),
      (LoadI32 (i32 (LO32F $val))),
      sub_even)>;  
*/
// Shuffle patterns
def : Pat<(int_npu_shufflef32 v16f32:$src0, v16i32:$src1),
  (SHUFFLEI_32 v16f32:$src0, v16i32:$src1)>;
/*def : Pat<(int_npu_shufflef64 v8f64:$src0, v8i64:$src1),
  (SHUFFLEI_64 v8f64:$src0, v8i64:$src1)>;
*/
// Splat patterns

def : Pat<(v16i32 (splat i32:$src0)), (MOVE_VS_32 i32:$src0)>;
//def : Pat<(v8i64 (splat i64:$src0)), (MOVE_VS_64 i64:$src0)>;
def : Pat<(v16f32 (splat f32:$src0)), (MOVE_VS_32 f32:$src0)>;
//def : Pat<(v8f64 (splat f64:$src0)), (MOVE_VS_64 f64:$src0)>;

def : Pat<(int_npu_vector_mixf32 v16f32:$src0, v16f32:$oldvalue),
          (MOVE_VV_M_32 v16f32:$src0, v16f32:$oldvalue)>;
/*def : Pat<(int_npu_vector_mixf64 v8f64:$src0, v8f64:$oldvalue),
          (MOVE_VV_M_64 v8f64:$src0, v8f64:$oldvalue)>;
*/

//---------------- Patterns for load and store ----------------//
//---------------- scalar load and store ----------------//
// zextload bool -> zextload byte
def : Pat<(i32 (zextloadi1_mem ADDRri:$addr)), (L32BU_Mainmem ADDRri:$addr)>;
//def : Pat<(i64 (zextloadi1_mem ADDRri:$addr)), (L64BU_Mainmem ADDRri:$addr)>;
def : Pat<(i32 (zextloadi1_scratch ADDRri:$addr)), (L32BU_Scratchpad ADDRri:$addr)>;
//def : Pat<(i64 (zextloadi1_scratch ADDRri:$addr)), (L64BU_Scratchpad ADDRri:$addr)>;

//---------------- scalar extload->zextload ----------------//
// Scalar load to 32-bit registers 
def : Pat<(i32 (extloadi1_mem ADDRri:$addr)), (L32BU_Mainmem ADDRri:$addr)>;
def : Pat<(i32 (extloadi8_mem ADDRri:$addr)), (L32BU_Mainmem ADDRri:$addr)>;
def : Pat<(i32 (extloadi16_mem ADDRri:$addr)), (L32SU_Mainmem ADDRri:$addr)>;
def : Pat<(f32 (MemLoad ADDRri:$addr)), (L32W_Mainmem ADDRri:$addr)>;
def : Pat<(i32 (extloadi1_scratch ADDRri:$addr)), (L32BU_Scratchpad ADDRri:$addr)>;
def : Pat<(i32 (extloadi8_scratch ADDRri:$addr)), (L32BU_Scratchpad ADDRri:$addr)>;
def : Pat<(i32 (extloadi16_scratch ADDRri:$addr)), (L32SU_Scratchpad ADDRri:$addr)>;
def : Pat<(f32 (ScratchpadLoad ADDRri:$addr)), (L32W_Scratchpad ADDRri:$addr)>;
// Scalar load to 64-bit registers 
/*
def : Pat<(i64 (extloadi1_mem ADDRri:$addr)), (L64BU_Mainmem ADDRri:$addr)>;
def : Pat<(i64 (extloadi8_mem ADDRri:$addr)), (L64BU_Mainmem ADDRri:$addr)>;
def : Pat<(i64 (extloadi16_mem ADDRri:$addr)), (L64SU_Mainmem ADDRri:$addr)>;
def : Pat<(i64 (extloadi32_mem ADDRri:$addr)), (L64WU_Mainmem ADDRri:$addr)>;
def : Pat<(f64 (MemLoad ADDRri:$addr)), (L64L_Mainmem ADDRri:$addr)>;
def : Pat<(i64 (extloadi1_scratch ADDRri:$addr)), (L64BU_Scratchpad ADDRri:$addr)>;
def : Pat<(i64 (extloadi8_scratch ADDRri:$addr)), (L64BU_Scratchpad ADDRri:$addr)>;
def : Pat<(i64 (extloadi16_scratch ADDRri:$addr)), (L64SU_Scratchpad ADDRri:$addr)>;
def : Pat<(i64 (extloadi32_scratch ADDRri:$addr)), (L64WU_Scratchpad ADDRri:$addr)>;
def : Pat<(f64 (ScratchpadLoad ADDRri:$addr)), (L64L_Scratchpad ADDRri:$addr)>;
*/
//---------------- scalar f32 load-> i32 load ----------------//
// Scalar store to 32-bit registers 
def : Pat<(MemStore f32:$dstsrc, ADDRri:$addr), (S32W_Mainmem f32:$dstsrc, ADDRri:$addr)>;
def : Pat<(ScratchpadStore f32:$dstsrc, ADDRri:$addr), (S32W_Scratchpad f32:$dstsrc, ADDRri:$addr)>;
// Scalar store to 64-bit registers 
//def : Pat<(MemStore f64:$dstsrc, ADDRri:$addr), (S64L_Mainmem f64:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore f64:$dstsrc, ADDRri:$addr), (S64L_Scratchpad f64:$dstsrc, ADDRri:$addr)>;

//---------------- vector load and store ----------------//
//---------------- intrinsics load and store ----------------//
//integer
def : Pat<(v16i32 (int_npu_block_loadv16i8 ADDRri:$addr)), (v16i32 (LV32BMainmem_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16i8_scratchpad ADDRri:$addr)), (v16i32 (LV32BScratchpad_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16u8 ADDRri:$addr)), (v16i32 (LV32BUMainmem_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16u8_scratchpad ADDRri:$addr)), (v16i32 (LV32BUScratchpad_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16i16 ADDRri:$addr)), (v16i32 (LV32SMainmem_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16i16_scratchpad ADDRri:$addr)), (v16i32 (LV32SScratchpad_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16u16 ADDRri:$addr)), (v16i32 (LV32SUMainmem_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16u16_scratchpad ADDRri:$addr)), (v16i32 (LV32SUScratchpad_U ADDRri:$addr))>;
def : Pat<(v16i32 (int_npu_block_loadv16i32_scratchpad ADDRri:$addr)), (v16i32 (LV32WScratchpad_U ADDRri:$addr))>;
/*def : Pat<(v8i64 (int_npu_block_loadv8i8 ADDRri:$addr)), (v8i64 (LV64BMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i8_scratchpad ADDRri:$addr)), (v8i64 (LV64BScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u8 ADDRri:$addr)), (v8i64 (LV64BUMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u8_scratchpad ADDRri:$addr)), (v8i64 (LV64BUScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i16 ADDRri:$addr)), (v8i64 (LV64SMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i16_scratchpad ADDRri:$addr)), (v8i64 (LV64SScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u16 ADDRri:$addr)), (v8i64 (LV64SUMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u16_scratchpad ADDRri:$addr)), (v8i64 (LV64SUScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i32 ADDRri:$addr)), (v8i64 (LV64WMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i32_scratchpad ADDRri:$addr)), (v8i64 (LV64WScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u32 ADDRri:$addr)), (v8i64 (LV64WUMainmem_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8u32_scratchpad ADDRri:$addr)), (v8i64 (LV64WUScratchpad_U ADDRri:$addr))>;
def : Pat<(v8i64 (int_npu_block_loadv8i64_scratchpad ADDRri:$addr)), (v8i64 (LV64LScratchpad_U ADDRri:$addr))>;
*/
def : Pat<(int_npu_block_storev16i8 ADDRri:$addr, v16i32:$dstsrc), (SV32BMainmem_U v16i32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16i8_scratchpad ADDRri:$addr, v16i32:$dstsrc), (SV32BScratchpad_U v16i32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16i16 ADDRri:$addr, v16i32:$dstsrc), (SV32SMainmem_U v16i32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16i16_scratchpad ADDRri:$addr, v16i32:$dstsrc), (SV32SScratchpad_U v16i32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16i32_scratchpad ADDRri:$addr, v16i32:$dstsrc), (SV32WScratchpad_U v16i32:$dstsrc, ADDRri:$addr)>;
/*def : Pat<(int_npu_block_storev8i8 ADDRri:$addr, v8i64:$dstsrc), (SV64BMainmem_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i8_scratchpad ADDRri:$addr, v8i64:$dstsrc), (SV64BScratchpad_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i16 ADDRri:$addr, v8i64:$dstsrc), (SV64SMainmem_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i16_scratchpad ADDRri:$addr, v8i64:$dstsrc), (SV64SScratchpad_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i32 ADDRri:$addr, v8i64:$dstsrc), (SV64WMainmem_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i32_scratchpad ADDRri:$addr, v8i64:$dstsrc), (SV64WScratchpad_U v8i64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8i64_scratchpad ADDRri:$addr, v8i64:$dstsrc), (SV64LScratchpad_U v8i64:$dstsrc, ADDRri:$addr)>;
*/
//float
def : Pat<(v16f32 (int_npu_block_loadv16f32_scratchpad ADDRri:$addr)), (LV32WScratchpad_U ADDRri:$addr)>;
def : Pat<(v16f32 (int_npu_block_loadv16f32_masked ADDRri:$addr)), (LV32WMainmem_M ADDRri:$addr)>;
def : Pat<(v16f32 (int_npu_block_loadv16f32_scratchpad_masked ADDRri:$addr)), (LV32WScratchpad_M ADDRri:$addr)>;
/*def : Pat<(v8f64 (int_npu_block_loadv8f64_scratchpad ADDRri:$addr)), (LV64LScratchpad_U ADDRri:$addr)>;
def : Pat<(v8f64 (int_npu_block_loadv8f64_masked ADDRri:$addr)), (LV64LMainmem_M ADDRri:$addr)>;
def : Pat<(v8f64 (int_npu_block_loadv8f64_scratchpad_masked ADDRri:$addr)), (LV64LScratchpad_M ADDRri:$addr)>;
*/
def : Pat<(int_npu_block_storev16f32_scratchpad ADDRri:$addr, v16f32:$dstsrc), (SV32WScratchpad_U v16f32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16f32_masked ADDRri:$addr, v16f32:$dstsrc), (SV32WMainmem_M v16f32:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev16f32_scratchpad_masked ADDRri:$addr, v16f32:$dstsrc), (SV32WScratchpad_M v16f32:$dstsrc, ADDRri:$addr)>;
/*
def : Pat<(int_npu_block_storev8f64_scratchpad ADDRri:$addr, v8f64:$dstsrc), (SV64LScratchpad_U v8f64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8f64_masked ADDRri:$addr, v8f64:$dstsrc), (SV64LMainmem_M v8f64:$dstsrc, ADDRri:$addr)>;
def : Pat<(int_npu_block_storev8f64_scratchpad_masked ADDRri:$addr, v8f64:$dstsrc), (SV64LScratchpad_M v8f64:$dstsrc, ADDRri:$addr)>;
*/
// Float gather/scatter
def : Pat<(int_npu_gather_loadf32_scratchpad V16ADDRri:$addr), (GLV32WScratchpad_U V16ADDRri:$addr)>;
def : Pat<(int_npu_gather_loadf32_scratchpad_masked V16ADDRri:$addr), (GLV32WScratchpad_M V16ADDRri:$addr)>;
/*
def : Pat<(int_npu_gather_loadf64_scratchpad V16ADDRri:$addr), (GLV64LScratchpad_U V16ADDRri:$addr)>;
def : Pat<(int_npu_gather_loadf64_scratchpad_masked V16ADDRri:$addr), (GLV64LScratchpad_M V16ADDRri:$addr)>;
*/
def : Pat<(int_npu_scatter_storef32_scratchpad V16ADDRri:$addr, v16f32:$srcDest), (SSV32WScratchpad_U v16f32:$srcDest, V16ADDRri:$addr)>;
def : Pat<(int_npu_scatter_storef32_scratchpad_masked V16ADDRri:$addr, v16f32:$srcDest), (SSV32WScratchpad_M v16f32:$srcDest, V16ADDRri:$addr)>;
/*
def : Pat<(int_npu_scatter_storef64_scratchpad V16ADDRri:$addr, v8f64:$srcDest), (SSV64LScratchpad_U v8f64:$srcDest, V16ADDRri:$addr)>;
def : Pat<(int_npu_scatter_storef64_scratchpad_masked V16ADDRri:$addr, v8f64:$srcDest), (SSV64LScratchpad_M v8f64:$srcDest, V16ADDRri:$addr)>;
*/
//---------------- vector anyextload -> sextload ----------------//
def : Pat<(v16i32 (anyextloadv16i8_mem ADDRri:$addr)), (LV32BMainmem_U ADDRri:$addr)>;
def : Pat<(v16i32 (anyextloadv16i8_scratch ADDRri:$addr)), (LV32BScratchpad_U ADDRri:$addr)>;
def : Pat<(v16i32 (anyextloadv16i16_mem ADDRri:$addr)), (LV32SMainmem_U ADDRri:$addr)>;
def : Pat<(v16i32 (anyextloadv16i16_scratch ADDRri:$addr)), (LV32SScratchpad_U ADDRri:$addr)>;
/*
def : Pat<(v8i64 (anyextloadv8i8_mem ADDRri:$addr)), (LV64BMainmem_U ADDRri:$addr)>;
def : Pat<(v8i64 (anyextloadv8i8_scratch ADDRri:$addr)), (LV64BScratchpad_U ADDRri:$addr)>;
def : Pat<(v8i64 (anyextloadv8i16_mem ADDRri:$addr)), (LV64SMainmem_U ADDRri:$addr)>;
def : Pat<(v8i64 (anyextloadv8i16_scratch ADDRri:$addr)), (LV64SScratchpad_U ADDRri:$addr)>;
def : Pat<(v8i64 (anyextloadv8i32_mem ADDRri:$addr)), (LV64WMainmem_U ADDRri:$addr)>;
def : Pat<(v8i64 (anyextloadv8i32_scratch ADDRri:$addr)), (LV64WScratchpad_U ADDRri:$addr)>;
*/
//---------------- vector load/store -> sextload/truncstore ----------------//
def : Pat<(v16i8 (MemLoad ADDRri:$addr)),  (LV32BMainmem_U ADDRri:$addr)>;
def : Pat<(v16i8 (ScratchpadLoad ADDRri:$addr)),  (LV32BScratchpad_U ADDRri:$addr)>;
def : Pat<(v16i16 (MemLoad ADDRri:$addr)), (LV32SMainmem_U ADDRri:$addr)>;
def : Pat<(v16i16 (ScratchpadLoad ADDRri:$addr)), (LV32SScratchpad_U ADDRri:$addr)>;
def : Pat<(v16i32 (ScratchpadLoad ADDRri:$addr)), (LV32WScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8f32 (MemLoad ADDRri:$addr)), (LV32WMainmem_U ADDRri:$addr)>;
//def : Pat<(v8f32 (ScratchpadLoad ADDRri:$addr)), (LV32WScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8i8 (MemLoad ADDRri:$addr)),  (LV64BMainmem_U ADDRri:$addr)>;
//def : Pat<(v8i16 (MemLoad ADDRri:$addr)), (LV64SMainmem_U ADDRri:$addr)>;
//def : Pat<(v8i32 (MemLoad ADDRri:$addr)), (LV64WMainmem_U ADDRri:$addr)>;
//def : Pat<(v8i8 (ScratchpadLoad ADDRri:$addr)),  (LV64BScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8i16 (ScratchpadLoad ADDRri:$addr)), (LV64SScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8i32 (ScratchpadLoad ADDRri:$addr)), (LV64WScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8i64 (ScratchpadLoad ADDRri:$addr)), (LV64LScratchpad_U ADDRri:$addr)>;
def : Pat<(MemStore v16i8:$dstsrc, ADDRri:$addr),  (SV32BMainmem_U v16i8:$dstsrc, ADDRri:$addr)>;
def : Pat<(MemStore v16i16:$dstsrc, ADDRri:$addr), (SV32SMainmem_U v16i16:$dstsrc, ADDRri:$addr)>;
def : Pat<(ScratchpadStore v16i8:$dstsrc, ADDRri:$addr),  (SV32BScratchpad_U v16i8:$dstsrc, ADDRri:$addr)>;
def : Pat<(ScratchpadStore v16i16:$dstsrc, ADDRri:$addr), (SV32SScratchpad_U v16i16:$dstsrc, ADDRri:$addr)>;
def : Pat<(ScratchpadStore v16i32:$dstsrc, ADDRri:$addr), (SV32WScratchpad_U v16i32:$dstsrc, ADDRri:$addr)>;
//def : Pat<(MemStore v8i8:$dstsrc, ADDRri:$addr),  (SV64BMainmem_U v8i8:$dstsrc, ADDRri:$addr)>;
//def : Pat<(MemStore v8i16:$dstsrc, ADDRri:$addr), (SV64SMainmem_U v8i16:$dstsrc, ADDRri:$addr)>;
//def : Pat<(MemStore v8i32:$dstsrc, ADDRri:$addr), (SV64WMainmem_U v8i32:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore v8i8:$dstsrc, ADDRri:$addr),  (SV64BScratchpad_U v8i8:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore v8i16:$dstsrc, ADDRri:$addr), (SV64SScratchpad_U v8i16:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore v8i32:$dstsrc, ADDRri:$addr), (SV64WScratchpad_U v8i32:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore v8i64:$dstsrc, ADDRri:$addr), (SV64LScratchpad_U v8i64:$dstsrc, ADDRri:$addr)>;

def : Pat<(v16f32 (MemLoad ADDRri:$addr)), (LV32WMainmem_U ADDRri:$addr)>;
def : Pat<(v16f32 (ScratchpadLoad ADDRri:$addr)), (LV32WScratchpad_U ADDRri:$addr)>;
//def : Pat<(v8f64 (MemLoad ADDRri:$addr)), (LV64LMainmem_U ADDRri:$addr)>;
//def : Pat<(v8f64 (ScratchpadLoad ADDRri:$addr)), (LV64LScratchpad_U ADDRri:$addr)>;
def : Pat<(MemStore v16f32:$dstsrc, ADDRri:$addr), (SV32WMainmem_U v16f32:$dstsrc, ADDRri:$addr)>;
def : Pat<(ScratchpadStore v16f32:$dstsrc, ADDRri:$addr), (SV32WScratchpad_U v16f32:$dstsrc, ADDRri:$addr)>;
//def : Pat<(MemStore v8f64:$dstsrc, ADDRri:$addr), (SV64LMainmem_U v8f64:$dstsrc, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore v8f64:$dstsrc, ADDRri:$addr), (SV64LScratchpad_U v8f64:$dstsrc, ADDRri:$addr)>;

// Questi pattern servono per gestire i tipi di dato vettoriali di dim < 512 bits
// v16i8 <=> v16i32
def : Pat<(v16i32 (bitconvert v16i8:$src)), (v16i32 (SEXT8_32VV_U v16i8:$src))>;
def : Pat<(MemStore (v16i8 (bitconvert v16i32:$src)), ADDRri:$addr), (SV32BMainmem_U v16i32:$src, ADDRri:$addr)>;
def : Pat<(ScratchpadStore (v16i8 (bitconvert v16i32:$src)), ADDRri:$addr), (SV32BScratchpad_U v16i32:$src, ADDRri:$addr)>;
// v16i16 <=> v16i32
def : Pat<(v16i32 (bitconvert v16i16:$src)), (v16i32 (SEXT16_32VV_U v16i16:$src))>;
def : Pat<(MemStore (v16i16 (bitconvert v16i32:$src)), ADDRri:$addr), (SV32SMainmem_U v16i32:$src, ADDRri:$addr)>;
def : Pat<(ScratchpadStore (v16i16 (bitconvert v16i32:$src)), ADDRri:$addr), (SV32SScratchpad_U v16i32:$src, ADDRri:$addr)>;
// v8i8 <=> v8i64
/*def : Pat<(v8i64 (bitconvert v8i8:$src)), (v8i64 (SEXT8_64VV_U v8i8:$src))>;
def : Pat<(MemStore (v8i8 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64BMainmem_U v8i64:$src, ADDRri:$addr)>;
def : Pat<(ScratchpadStore (v8i8 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64BScratchpad_U v8i64:$src, ADDRri:$addr)>;
// v8i16 <=> v8i64
def : Pat<(v8i64 (bitconvert v8i16:$src)), (v8i64 (SEXT16_64VV_U v8i16:$src))>;
def : Pat<(MemStore (v8i16 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64SMainmem_U v8i64:$src, ADDRri:$addr)>;
def : Pat<(ScratchpadStore (v8i16 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64SScratchpad_U v8i64:$src, ADDRri:$addr)>;
// v8i32 <=> v8i64
def : Pat<(v8i64 (bitconvert v8i32:$src)), (v8i64 (SEXT32_64VV_U v8i32:$src))>;
def : Pat<(MemStore (v8i32 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64WMainmem_U v8i64:$src, ADDRri:$addr)>;
def : Pat<(ScratchpadStore (v8i32 (bitconvert v8i64:$src)), ADDRri:$addr), (SV64WScratchpad_U v8i64:$src, ADDRri:$addr)>;
*/
// v8f32 <=> v16f32
//def : Pat<(MemStore (v8f32 (extract_subvector v16f32:$src, (i32 0))), ADDRri:$addr), (SV32WMainmem_U v16f32:$src, ADDRri:$addr)>;
//def : Pat<(ScratchpadStore (v8f32 (extract_subvector v16f32:$src, (i32 0))), ADDRri:$addr), (SV32WScratchpad_U v16f32:$src, ADDRri:$addr)>;



//---------------- Patterns for type conversions ----------------//

// f32 -> i64: f32 -> i32 -> i64
//def : Pat<(i64 (fp_to_sint (f32 GPR32:$src))), (i64 (SEXT32_64SS (F32TOI32_SS GPR32:$src)))>;
// f64 -> i64: f64 -> f32 -> i32 -> i64
//def : Pat<(i64 (fp_to_sint (f64 GPR64:$src))), (i64 (SEXT32_64SS (F32TOI32_SS (DTOF_SS GPR32:$src))))>;
// f64 -> i32: f64 -> f32 -> i32
//def : Pat<(i32 (fp_to_sint (f64 GPR64:$src))), (i32 (F32TOI32_SS (DTOF_SS GPR32:$src)))>;
// i32 -> f64: i32 -> f32 -> f64
//def: Pat<(f64 (sint_to_fp (i32 GPR32:$src))), (FTOD_SS (I32TOF32_SS GPR32:$src))>;
// i64 -> f32: i64 -> i32 -> f32
//def: Pat<(f32 (sint_to_fp (i64 GPR64:$src))), (I32TOF32_SS (GetEvenReg GPR64:$src))>;
// i64 -> f64: i64 -> i32 -> f32 -> f64
//def: Pat<(f64 (sint_to_fp (i64 GPR64:$src))), (FTOD_SS (I32TOF32_SS (GetEvenReg GPR64:$src)))>;

//v8i64 -> v8f64
//def: Pat<(v8f64 (sint_to_fp (v8i64 VR512L:$src))), (I64TOF64_VV_U VR512L:$src)>;


// Trunc operation from i64 to i32
//def: Pat<(i32 (trunc (i64 GPR64:$src))), (GetEvenReg GPR64:$src)>;

// Sign Extension Patterns sext_inreg -> sext
def : Pat<(v16i32 (sext_inreg v16i32:$src, v16i8)), (v16i32 (SEXT8_32VV_U v16i32:$src))>;
def : Pat<(v16i32 (sext_inreg v16i32:$src, v16i16)), (v16i32 (SEXT16_32VV_U v16i32:$src))>;
//def : Pat<(v8i64 (sext_inreg v8i64:$src, v8i8)), (v8i64 (SEXT8_64VV_U v8i64:$src))>;
//def : Pat<(v8i64 (sext_inreg v8i64:$src, v8i16)), (v8i64 (SEXT16_64VV_U v8i64:$src))>;
//def : Pat<(v8i64 (sext_inreg v8i64:$src, v8i32)), (v8i64 (SEXT32_64VV_U v8i64:$src))>;

// BITCAST patterns
def : Pat<(f32 (bitconvert (i32 GPR32:$src))), (f32 GPR32:$src)>;
def : Pat<(i32 (bitconvert (f32 GPR32:$src))), (i32 GPR32:$src)>;
//def : Pat<(f64 (bitconvert (i64 GPR64:$src))), (f64 GPR64:$src)>;
//def : Pat<(i64 (bitconvert (f64 GPR64:$src))), (i64 GPR64:$src)>;
def : Pat<(v16f32 (bitconvert (v16i32 VR512W:$src))), (v16f32 VR512W:$src)>;
def : Pat<(v16i32 (bitconvert (v16f32 VR512W:$src))), (v16i32 VR512W:$src)>;
//def : Pat<(v8f64 (bitconvert (v8i64 VR512L:$src))), (v8f64 VR512L:$src)>;
//def : Pat<(v8i64 (bitconvert (v8f64 VR512L:$src))), (v8i64 VR512L:$src)>;

// Pattern added to support v8f32
//def : Pat<(v8i64 (bitconvert (v16f32 VR512W:$src))), (v8i64 VR512L:$src)>;
//def : Pat<(v16f32 (bitconvert (v8i64 VR512L:$src))), (v16f32 VR512W:$src)>;
//miodef : Pat<(v8f32 (bitconvert (v16f32 VR512W:$src))), (v8f32 VR512W:$src)>;


// CLZ and CTZ Intrinsics
def : Pat<(i32 (int_npu_clzi32 i32:$src)),
(i32 (CLZSS_32 i32:$src))>;
/*def : Pat<(i64 (int_npu_clzi64 i64:$src)),
(i64 (CLZSS_64 i64:$src))>;
*/
def : Pat<(v16i32 (int_npu_clzv16i32 v16i32:$src)),
(v16i32 (CLZVV_U_32 v16i32:$src))>;
/*
def : Pat<(v8i64 (int_npu_clzv8i64 v8i64:$src)),
(v8i64 (CLZVV_U_64 v8i64:$src))>;
*/
def : Pat<(i32 (int_npu_ctzi32 i32:$src)),
(i32 (CTZSS_32 i32:$src))>;
/*
def : Pat<(i64 (int_npu_ctzi64 i64:$src)),
(i64 (CTZSS_64 i64:$src))>;
*/
def : Pat<(v16i32 (int_npu_ctzv16i32 v16i32:$src)),
(v16i32 (CTZVV_U_32 v16i32:$src))>;
/*
def : Pat<(v8i64 (int_npu_ctzv8i64 v8i64:$src)),
(v8i64 (CTZVV_U_64 v8i64:$src))>;
*/
// Integer division
// v16i32
def : Pat<(v16i32 (sdiv v16i32:$src1, v16i32:$src2)),
    (F32TOI32_VV_U (DIVFVVV_U_32 (I32TOF32_VV_U v16i32:$src1), (I32TOF32_VV_U v16i32:$src2)))>;

// Branch patterns
def : Pat<(brcond (i32 (setne i32:$lhs, 0)), bb:$addr), (BNEZ i32:$lhs, bb:$addr)>;
def : Pat<(brcond (i32 (seteq i32:$lhs, 0)), bb:$addr), (BEQZ i32:$lhs, bb:$addr)>;
def : Pat<(brcond i32:$lhs, bb:$addr), (BNEZ i32:$lhs, bb:$addr)>;
//def : Pat<(brcond (i64 (setne i64:$lhs, 0)), bb:$addr), (BNEZ i64:$lhs, bb:$addr)>;
//def : Pat<(brcond (i64 (seteq i64:$lhs, 0)), bb:$addr), (BEQZ i64:$lhs, bb:$addr)>;
//def : Pat<(brcond i64:$lhs, bb:$addr), (BNEZ i64:$lhs, bb:$addr)>;

// Call patterns
def : Pat<(call tglobaladdr:$addr), (JMPSR_OFF tglobaladdr:$addr)>;
def : Pat<(call texternalsym:$addr), (JMPSR_OFF texternalsym:$addr)>;

// GetLane patterns
def : Pat<(extractelt v16f32:$src0, i32:$src1),
          (GET_LANEI_32 v16f32:$src0, i32:$src1)>;
def : Pat<(extractelt v16f32:$src, simm9:$imm),
          (GET_LANEIimm v16f32:$src, imm:$imm)>;
/*def : Pat<(extractelt v8f64:$src0, i32:$src1),
          (GET_LANEI_64 v8f64:$src0, i32:$src1)>;
*/
// This pattern is necessary due to type legalizer which tries to bitcast v8i64 into v16i32 
/*def : Pat<(extractelt (v16i32 (bitconvert (v8i64 VR512L:$src0))), simm9:$imm),
          (GetEvenReg (GET_LANEI_64 v8i64:$src0, (LoadI32 (i32 (DIV2 simm9:$imm)))))>;
*/
// SEXT ZEXT ANYEXT i32 -> i64
//def : Pat<(i64 (sext i32:$val)), (SEXT32_64SS (SetEvenReg i32:$val))>;
//def : Pat<(i64 (anyext i32:$val)), (SEXT32_64SS (SetEvenReg i32:$val))>;
//def : Pat<(i64 (zext i32:$val)), (ANDSSS_64 (i64 (SetEvenReg i32:$val)), (i64 0x00000000FFFFFFFF))>;

// ISD Nodes Patterns
def : Pat<(i32 (leah tglobaladdr:$addr)), (LEAH tglobaladdr:$addr)>;
def : Pat<(i32 (leal i32:$addrh, tglobaladdr:$addr)), (LEAL i32:$addrh, tglobaladdr:$addr)>;
def : Pat<(i32 (leah tconstpool:$addr)), (LEAH tconstpool:$addr)>;
def : Pat<(i32 (leal i32:$addrh, tconstpool:$addr)), (LEAL i32:$addrh, tconstpool:$addr)>;
def : Pat<(i32 (leah tblockaddress:$addr)), (LEAH tblockaddress:$addr)>;
def : Pat<(i32 (leal i32:$addrh, tblockaddress:$addr)), (LEAL i32:$addrh, tblockaddress:$addr)>;
def : Pat<(i32 (leah tjumptable:$addr)), (LEAH tjumptable:$addr)>;
def : Pat<(i32 (leal i32:$addrh, tjumptable:$addr)), (LEAL i32:$addrh, tjumptable:$addr)>;
