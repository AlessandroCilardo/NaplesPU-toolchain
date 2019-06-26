//===-- NaplesPUTargetMachine.h - Define TargetMachine for NaplesPU ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the NaplesPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPUTARGETMACHINE_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPUTARGETMACHINE_H

#include "NaplesPUSubtarget.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class NaplesPUTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  NaplesPUSubtarget Subtarget;

public:
  NaplesPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT);

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  const NaplesPUSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  bool isMachineVerifierClean() const override {
    return false;
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) override;
};

} // end namespace llvm

#endif
