//===-- NaplesPUbaseInfo.h - Top level definitions for NaplesPU MC ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_MCTARGETDESC_NAPLESPUBASEINFO_H
#define LLVM_LIB_TARGET_NAPLESPU_MCTARGETDESC_NAPLESPUBASEINFO_H

namespace llvm {

namespace NaplesPU {
// Target Operand Flag enum.
enum TOF {
  //===------------------------------------------------------------------===//
  MO_NO_FLAG,

  // MO_ABS_HI/LO - Represents the hi or low part of an absolute symbol
  // address.
  MO_ABS_HI,
  MO_ABS_LO,
  MO_GOT
};
} // namespace NaplesPU
} // namespace llvm
#endif // LLVM_LIB_TARGET_NAPLESPU_MCTARGETDESC_NAPLESPUBASEINFO_H
