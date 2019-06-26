//===--- NaplesPUTargetObjectFile.h - NaplesPU Object Info ---------*- C++ ----*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file deals with any NaplesPU specific requirements on object files.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_NAPLESPU_TARGETOBJECTFILE_H
#define LLVM_TARGET_NAPLESPU_TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
// Questa classe dovrebbe indicare l'uso del formato ELF per il file oggetto
class NaplesPUTargetObjectFile : public TargetLoweringObjectFileELF {
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif
