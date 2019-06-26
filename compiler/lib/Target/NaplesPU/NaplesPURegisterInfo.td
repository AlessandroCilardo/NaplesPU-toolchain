//===-- NaplesPURegisterInfo.td - NaplesPU Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the NaplesPU register file
//===----------------------------------------------------------------------===//

let Namespace = "NaplesPU" in {
def sub_even : SubRegIndex<32>;
def sub_odd  : SubRegIndex<32, 32>;
}

class NaplesPUReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "NaplesPU";
}

class NaplesPURegWithSubRegs<bits<16> Enc, string n, list<Register> subregs>
  : RegisterWithSubRegs<n, subregs> {
  let HWEncoding = Enc;
  let Namespace = "NaplesPU";
}

// NaplesPU 32-bit registers
class NaplesPUGPRReg<bits<16> Enc, string n> : NaplesPUReg<Enc, n>;
/*
// NaplesPU pairs of 32-bit registers
class NaplesPU64GPRReg<bits<16> Enc, string n, list<Register> subregs>
  : NaplesPURegWithSubRegs<Enc, n, subregs> {
  let SubRegIndices = [sub_even, sub_odd];
  let CoveredBySubRegs = 1;
}

*/
//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//


  // General Purpose scalar registers
  foreach i = 0-57 in {
    def S#i : NaplesPUGPRReg<i, "s"#i>, DwarfRegNum<[i]>;
  }

  //trap register
  def TR_REG : NaplesPUGPRReg<58, "tr">, DwarfRegNum<[58]>;
  //mask register
  def MR_REG : NaplesPUGPRReg<59, "rm">, DwarfRegNum<[59]>;
  //frame pointer
  def FP_REG : NaplesPUGPRReg<60, "fp">, DwarfRegNum<[60]>;
  //stack pointer
  def SP_REG : NaplesPUGPRReg<61, "sp">, DwarfRegNum<[61]>;
  //return address
  def RA_REG : NaplesPUGPRReg<62, "ra">, DwarfRegNum<[62]>;
  //PC
  def PC_REG : NaplesPUGPRReg<63, "pc">, DwarfRegNum<[63]>;

  // General Purpose vectorial registers
  foreach i = 0-63 in {
    def V#i : NaplesPUGPRReg<i, "v"#i>, DwarfRegNum<[!add(i, 64)]>;
  }
/*
  // Aliases of the scalar registers used for double-word operations 
  foreach i = 0-28 in {
    def S#!shl(i, 1)#_S#!add(!shl(i, 1), 1) : NaplesPU64GPRReg<!shl(i, 1), "s"#!shl(i, 1)#_64,
                 [!cast<NaplesPUGPRReg>("S"#!shl(i, 1)),
                  !cast<NaplesPUGPRReg>("S"#!add(!shl(i, 1), 1))]>;
  }
*/

//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

// This register class should not be used to hold i64/f64 values, use the GPR64
// register class for that. The i64/f64 type is included here to allow i64/f64
// patterns using the instructions.
def GPR32 : RegisterClass<"NaplesPU", [i32, f32, i64, f64], 32, (add (sequence "S%u", 0, 57),
  TR_REG, MR_REG, FP_REG, SP_REG, RA_REG, PC_REG)>;

// Register class for 64-bit mode, with a 64-bit spill slot size.
// These are the same as the 32-bit registers, so TableGen will consider this
// to be a sub-class of GPR32. That works out because requiring a 64-bit
// spill slot is a stricter constraint than only requiring a 32-bit spill slot.
//def GPR64 : RegisterClass<"NaplesPU", [i64, f64], 64, (add GPR32)>;

// Should be in the same order as GPR32.
/*def GPR64 : RegisterClass<"NaplesPU", [i64, f64], 64,
    (add S0_S1, S2_S3, S4_S5, S6_S7,
         S8_S9, S10_S11, S12_S13, S14_S15,
         S16_S17, S18_S19, S20_S21, S22_S23,
         S24_S25, S26_S27, S28_S29, S30_S31,
         S32_S33, S34_S35, S36_S37, S38_S39,
         S40_S41, S42_S43, S44_S45, S46_S47,
         S48_S49, S50_S51, S52_S53, S54_S55,
         S56_S57)>;
*/
def VR512W : RegisterClass<"NaplesPU", [v16i32, v16f32, v16i8, v16i16], 512, (sequence "V%u", 0, 63)>;
//def VR512L : RegisterClass<"NaplesPU", [v8i64, v8f64, v8i8, v8i16, v8i32], 512, (sequence "V%u", 0, 63)>;


