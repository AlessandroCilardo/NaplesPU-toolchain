//==- NaplesPUMachineFunctionInfo.h - NaplesPU Machine Function Info -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares  NaplesPU specific per-machine-function information.
//
//===----------------------------------------------------------------------===//
#ifndef NAPLESPUMACHINEFUNCTIONINFO_H
#define NAPLESPUMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class NaplesPUMachineFunctionInfo : public MachineFunctionInfo {
public:
  NaplesPUMachineFunctionInfo() : SRetReturnReg(0) {}

  explicit NaplesPUMachineFunctionInfo(MachineFunction &MF) : SRetReturnReg(0) {}

  unsigned getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(unsigned Reg) { SRetReturnReg = Reg; }
  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

private:
  virtual void anchor();

  /// SRetReturnReg - Holds the virtual register into which the sret
  /// argument is passed.
  unsigned SRetReturnReg;
  int VarArgsFrameIndex;
};
}

#endif
