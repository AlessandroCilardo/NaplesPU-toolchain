//===-- NaplesPUMCInstLower.h - Lower MachineInstr to MCInst -------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPUMCINSTLOWER_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPUMCINSTLOWER_H
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
  class MCContext;
  class MCInst;
  class MCOperand;
  class MachineInstr;
  class MachineFunction;
  class NaplesPUAsmPrinter;
  //TODO: a che serve Mangler?
  class Mangler;

/// NaplesPUMCInstLower - This class is used to lower an MachineInstr into an
//                    MCInst.
class LLVM_LIBRARY_VISIBILITY NaplesPUMCInstLower {
  typedef MachineOperand::MachineOperandType MachineOperandType;
  MCContext *Ctx;
  NaplesPUAsmPrinter &AsmPrinter;
public:
  NaplesPUMCInstLower(NaplesPUAsmPrinter &asmprinter);
  void Initialize(MCContext *C);
  void Lower(const MachineInstr *MI, MCInst &OutMI) const;
  MCOperand LowerOperand(const MachineOperand& MO, unsigned offset = 0) const;

private:
  MCOperand LowerSymbolOperand(const MachineOperand &MO,
                               MachineOperandType MOTy, unsigned Offset) const;
};
}

#endif