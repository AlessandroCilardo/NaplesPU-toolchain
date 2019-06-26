//===--- NaplesPU.h - NaplesPU ToolChain Implementations ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_NAPLESPU_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_NAPLESPU_H

#include "Gnu.h"
#include "clang/Driver/Tool.h"
#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace tools {
namespace naplespu {
class LLVM_LIBRARY_VISIBILITY Linker : public Tool {
public:
  Linker(const ToolChain &TC) : Tool("naplespu::Linker",
    "naplespu-ld", TC) {}

  virtual bool hasIntegratedCPP() const override { return false; }
  virtual bool isLinkJob() const override { return true; }
  virtual void ConstructJob(Compilation &C, const JobAction &JA,
                            const InputInfo &Output,
                            const InputInfoList &Inputs,
                            const llvm::opt::ArgList &TCArgs,
                            const char *LinkingOutput) const override;
};
} // end namespace naplespu.
} // end namespace tools


namespace toolchains {

class LLVM_LIBRARY_VISIBILITY NaplesPUToolChain : public ToolChain {
public:
  NaplesPUToolChain(const Driver &D, const llvm::Triple &Triple,
                      const llvm::opt::ArgList &Args);
  ~NaplesPUToolChain();
  RuntimeLibType GetRuntimeLibType(const llvm::opt::ArgList &Args) const override;
  bool IsIntegratedAssemblerDefault() const override;
  bool isPICDefault() const override;
  bool isPIEDefault() const override;
  bool isPICDefaultForced() const override;
  void addClangTargetOptions(const llvm::opt::ArgList &DriverArgs,
     llvm::opt::ArgStringList &CC1Args,
     Action::OffloadKind DeviceOffloadKind) const override;
  bool IsUnwindTablesDefault(const llvm::opt::ArgList &Args) const override;

protected:
  Tool *buildLinker() const override;
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_NAPLESPU_H

