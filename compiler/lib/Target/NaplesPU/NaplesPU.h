//===-- NaplesPU.h - Top-level interface for NaplesPU representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// NaplesPU back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPU_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPU_H

#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class FunctionPass;
  class NaplesPUTargetMachine;
  class formatted_raw_ostream;

  FunctionPass *createNaplesPUISelDag(NaplesPUTargetMachine &TM);

} // end namespace llvm;

#endif
