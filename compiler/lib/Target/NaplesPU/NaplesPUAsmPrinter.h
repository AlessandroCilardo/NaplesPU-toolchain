//===-- NaplesPUAsmPrinter.h - NaplesPU LLVM Assembly Printer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// NaplesPU Assembly printer class.
//
// The name is a bit misleading.  Because we use the MC layer for code
// generation, the job of this class is now mostly to convert MachineInstrs
// into MCInsts. Most of the work is done by a helper class MCInstLowering
// (which in turn uses code generated by TableGen).  This also performs
// replacement of inline assembly parameters.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPUASMPRINTER_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPUASMPRINTER_H

#include "NaplesPUMCInstLower.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class MCStreamer;
class MachineInstr;
class MachineBasicBlock;
class Module;
class raw_ostream;

class LLVM_LIBRARY_VISIBILITY NaplesPUAsmPrinter : public AsmPrinter {
  NaplesPUMCInstLower MCInstLowering;

public:
  explicit NaplesPUAsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(*this) {}

  StringRef getPassName() const override { return "NaplesPU Assembly Printer"; }

  void EmitInstruction(const MachineInstr *MI) override;

  /// Targets can override this to emit stuff before the first basic block in
  /// the function.
  void EmitFunctionBodyStart() override;

  /// Print to the current output stream assembly representations of the
  /// constants in the constant pool MCP. This is used to print out constants
  /// which have been "spilled to memory" by the code generator.
  void EmitConstantPool() override;

  // Print operand for inline assembly
  //
    /// Print the specified operand of MI, an INLINEASM instruction, using the
    /// specified assembler variant.  Targets should override this to format as
    /// appropriate.  This method can return true if the operand is erroneous.
  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       unsigned AsmVariant, const char *ExtraCode,
                       raw_ostream &O) override;

   /// Print the specified operand of MI, an INLINEASM instruction, using the
   /// specified assembler variant as an address. Targets should override this to
   /// format as appropriate.  This method can return true if the operand is
   /// erroneous.
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             unsigned AsmVariant, const char *ExtraCode,
                             raw_ostream &O) override;
};
}

#endif
