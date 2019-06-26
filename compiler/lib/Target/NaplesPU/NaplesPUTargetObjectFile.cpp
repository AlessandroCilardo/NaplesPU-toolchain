//===------ NaplesPUTargetObjectFile.cpp - NaplesPU Object Info -----------------===//
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

#include "NaplesPUTargetObjectFile.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void NaplesPUTargetObjectFile::Initialize(MCContext &Ctx,
                                       const TargetMachine &TM) {
// Funzione del padre TargetLoweringObjectFile
/// Initialize - this method must be called before any actual lowering is
/// done.  This specifies the current context for codegen, and gives the
/// lowering implementations a chance to set up their default sections.
  TargetLoweringObjectFileELF::Initialize(Ctx, TM);
  
  InitializeELF(TM.Options.UseInitArray);
}
