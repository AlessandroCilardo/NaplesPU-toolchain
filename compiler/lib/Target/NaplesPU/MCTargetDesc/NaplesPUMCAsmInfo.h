//===-- NaplesPUMCAsmInfo.h - NaplesPU asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPUMCASMINFO_H
#define LLVM_LIB_TARGET_NAPLESPUMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class NaplesPUMCAsmInfo : public MCAsmInfo {
  virtual void anchor();

public:
  explicit NaplesPUMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
