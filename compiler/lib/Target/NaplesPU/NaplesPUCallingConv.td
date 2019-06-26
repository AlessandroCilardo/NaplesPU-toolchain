//===-- NaplesPUCallingConv.td - Calling Conventions NaplesPU ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the NaplesPU architectures.
//
//===----------------------------------------------------------------------===//

def CC_NaplesPU32 : CallingConv<[

  // TODO: (Catello) maybe we must add the vector types < 512 bits
  // i1, i8 and i16 integers are promoted to i32 by the caller.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  CCIfType<[v16i8, v16i16], CCPromoteToType<v16i32>>,
  CCIfType<[v8i8, v8i16, v8i32], CCPromoteToType<v8i64>>,

  // i32 f32 arguments get passed in integer registers if there is space.
  CCIfNotVarArg<CCIfType<[i32, f32], CCAssignToReg<[S0, S1, S2, S3, S4, S5, S6, S7]>>>,

  //TODO: controllare se modificare e usare registri con split come nello SPARC

/*  CCIfNotVarArg<CCIfType<[i64, f64], CCAssignToReg<[S0_S1, S2_S3, S4_S5, S6_S7, S8_S9, S10_S11, S12_S13, S14_S15]>>>,
*/
  // Vector arguments can be passed in their own registers, as above
  CCIfNotVarArg<CCIfType<[v16i32, v16f32/*, v8i64, v8f64*/], CCAssignToReg<[V0, V1, V2, V3, V4, V5, V6, V7]>>>,

  // Stick remaining registers onto stack, aligned by size
  CCIfType<[i32, f32], CCAssignToStack<4, 4>>,
//  CCIfType<[i64, f64], CCAssignToStack<8, 8>>,
  CCIfType<[v16i32, v16f32/*, v8i64, v8f64*/], CCAssignToStack<64, 64>>
]>;

def RetCC_NaplesPU32 : CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  CCIfType<[i32, f32], CCAssignToReg<[S0, S1, S2, S3, S4, S5]>>,

//  CCIfType<[i64, f64], CCAssignToReg<[S0_S1, S2_S3, S4_S5, S6_S7, S8_S9, S10_S11]>>,

  CCIfType<[v16i8, v16i16], CCPromoteToType<v16i32>>,
//  CCIfType<[v8i8, v8i16, v8i32], CCPromoteToType<v8i64>>,

  CCIfType<[v16i32, v16f32/*, v8i64, v8f64*/], CCAssignToReg<[V0, V1, V2, V3, V4, V5]>>
]>;

//TODO CONTROLLARE PERCHÈ DA 50 A 57 E DA 56 A 63 e i reg speciali
//Callee only needs to save the callee-save registers that are used in the body of its subroutine.
def NaplesPUCSR : CalleeSavedRegs<(add (sequence "S%u", 50, 57), MR_REG, FP_REG, RA_REG,
                               (sequence "V%u", 56, 63))>;
