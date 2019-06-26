//===--- NaplesPU.cpp - Implement NaplesPU target feature support ---------------===//
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

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_NAPLESPU_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_NAPLESPU_H

#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY NaplesPUTargetInfo : public TargetInfo {
  static const char *const GCCRegNames[];
  static const Builtin::Info BuiltinInfo[];
public:
  NaplesPUTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
    : TargetInfo(Triple) {
    BigEndian = false;
    TLSSupported = false;
    IntWidth = IntAlign = 32;
    PointerWidth = PointerAlign = 32;
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 32;
    resetDataLayout("e-m:e-p:32:32");
    LongDoubleWidth = 32;
    LongDoubleAlign = 32;
    DoubleWidth = 32;
    DoubleAlign = 32;
    DoubleFormat = &llvm::APFloat::IEEEsingle();
    LongDoubleFormat = &llvm::APFloat::IEEEsingle();
  }

  bool setCPU(const std::string &Name) override {
    return Name == "naplespu";
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override {
    Builder.defineMacro("__NAPLESPU__");
  }

  ArrayRef<Builtin::Info> getTargetBuiltins() const override;
  ArrayRef<const char*> getGCCRegNames() const override;

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return None;
  }

  bool validateAsmConstraint(const char *&Name,
                                     TargetInfo::ConstraintInfo &info) const override;
  const char *getClobbers() const override {
    return "";
  }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }
};

} // namespace targets
} // namespace clang

#endif 

