//===-- NaplesPUTargetMachine.cpp - Define TargetMachine for NaplesPU -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "NaplesPUTargetMachine.h"
#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "NaplesPU.h"
#include "NaplesPUTargetObjectFile.h"
#include "NaplesPUTargetTransformInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

extern "C" void LLVMInitializeNaplesPUTarget() {
  // Register the target.
  RegisterTargetMachine<NaplesPUTargetMachine> X(TheNaplesPUTarget);
}

namespace {
/// NaplesPU Code Generator Pass Configuration Options.
class NaplesPUPassConfig : public TargetPassConfig {
public:
  NaplesPUPassConfig(NaplesPUTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  NaplesPUTargetMachine &getNaplesPUTargetMachine() const {
    return getTM<NaplesPUTargetMachine>();
  }

  bool addInstSelector() override;
};

Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                    Optional<Reloc::Model> RM) {
  if (!RM.hasValue() || *RM == Reloc::DynamicNoPIC)
    return Reloc::Static;

  return *RM;
}

} // namespace

NaplesPUTargetMachine::NaplesPUTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, "e-m:e-p:32:32", TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Medium), OL),
      TLOF(make_unique<NaplesPUTargetObjectFile>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

TargetPassConfig *NaplesPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new NaplesPUPassConfig(*this, PM);
}

bool NaplesPUPassConfig::addInstSelector() {
  addPass(createNaplesPUISelDag(getNaplesPUTargetMachine()));
  return false;
}

TargetTransformInfo NaplesPUTargetMachine::getTargetTransformInfo(const Function &F) {
  return TargetTransformInfo(NaplesPUTTIImpl(this, F));
}
