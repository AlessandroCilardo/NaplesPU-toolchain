//===--- NaplesPU.cpp - NaplesPU ToolChain Implementations ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "NaplesPU.h"
#include "Arch/ARM.h"
#include "Arch/Mips.h"
#include "Arch/Sparc.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

void naplespu::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  ArgStringList CmdArgs;
  const ToolChain &TC = getToolChain();

  if (Output.isFilename()) {
    CmdArgs.push_back("-o");
    CmdArgs.push_back(Output.getFilename());
  } else {
    assert(Output.isNothing() && "Invalid output.");
  }

  AddLinkerInputs(TC, Inputs, Args, CmdArgs, JA);
  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nodefaultlibs))
    AddRunTimeLibs(TC, TC.getDriver(), CmdArgs, Args);

  const char *Exec = Args.MakeArgString(getToolChain().GetProgramPath("ld.lld"));
  C.addCommand(llvm::make_unique<Command>(JA, *this, Exec, CmdArgs, Inputs));
}

NaplesPUToolChain::NaplesPUToolChain(const Driver &D, const llvm::Triple &Triple,
                               const llvm::opt::ArgList &Args)
	:	ToolChain(D, Triple, Args)
{
  getProgramPaths().push_back(getDriver().getInstalledDir());
  if (getDriver().getInstalledDir() != getDriver().Dir)
    getProgramPaths().push_back(getDriver().Dir);
}

NaplesPUToolChain::~NaplesPUToolChain()
{
}

ToolChain::RuntimeLibType
NaplesPUToolChain::GetRuntimeLibType(const llvm::opt::ArgList &Args) const
{
  if (Arg *A = Args.getLastArg(clang::driver::options::OPT_rtlib_EQ)) {
    StringRef Value = A->getValue();
    if (Value != "compiler-rt")
      getDriver().Diag(clang::diag::err_drv_invalid_rtlib_name)
          << A->getAsString(Args);
  }

  return ToolChain::RLT_CompilerRT;
}

bool NaplesPUToolChain::IsIntegratedAssemblerDefault() const
{
  return true;
}

bool NaplesPUToolChain::isPICDefault() const
{
  return false;
}

bool NaplesPUToolChain::isPIEDefault() const
{
  return false;
}

bool NaplesPUToolChain::isPICDefaultForced() const
{
  return false;
}

void NaplesPUToolChain::addClangTargetOptions(const ArgList &DriverArgs,
                                  ArgStringList &CC1Args,
                                  Action::OffloadKind DeviceOffloadKind) const {
  CC1Args.push_back("-nostdsysteminc");
  if (DriverArgs.hasFlag(options::OPT_fuse_init_array,
                         options::OPT_fno_use_init_array,
                         true))
  {
    CC1Args.push_back("-fuse-init-array");
  }
}

// Emit .eh_frame to allow stack unwinding.
bool NaplesPUToolChain::IsUnwindTablesDefault(const ArgList &Args) const {
  return true;
}

Tool *NaplesPUToolChain::buildLinker() const {
  return new tools::naplespu::Linker(*this);
}

