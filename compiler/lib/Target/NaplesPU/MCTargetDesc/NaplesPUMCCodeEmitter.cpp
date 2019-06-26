//===-- NaplesPUMCCodeEmitter.cpp - Convert NaplesPU code to machine code -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the NaplesPUMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/NaplesPUFixupKinds.h"
#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {
class NaplesPUMCCodeEmitter : public MCCodeEmitter {
  NaplesPUMCCodeEmitter(const NaplesPUMCCodeEmitter &) = delete;
  void operator=(const NaplesPUMCCodeEmitter &) = delete;
  MCContext &Ctx;

public:
  NaplesPUMCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx): Ctx(ctx) {}

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  unsigned encodeMemoryOpValue(const MCInst &MI, unsigned Op,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const;

  unsigned encodeLEAValue(const MCInst &MI, unsigned Op,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;

  unsigned encodeABShValue(const MCInst &MI, unsigned Op,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const; 

  unsigned encodeABSlValue(const MCInst &MI, unsigned Op,
                          SmallVectorImpl<MCFixup> &Fixups,
                          const MCSubtargetInfo &STI) const;                        

  unsigned encodeBranchTargetOpValue(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const;
  
  // Emit one byte through output stream (from MCBlazeMCCodeEmitter)
  void EmitByte(unsigned char C, raw_ostream &OS) const { OS << (char)C; }

  void EmitLEConstant(uint64_t Val, unsigned Size, raw_ostream &OS) const {
    assert(Size <= 8 && "size too big in emit constant");

    for (unsigned i = 0; i != Size; ++i) {
      EmitByte(Val & 255, OS);
      Val >>= 8;
    }
  }

};
} // end anonymous namespace

MCCodeEmitter *llvm::createNaplesPUMCCodeEmitter(const MCInstrInfo &MCII,
                                              const MCRegisterInfo &MRI,
                                              MCContext &Ctx) {
  return new NaplesPUMCCodeEmitter(MCII, Ctx);
}

// Return binary encoding of operand.
unsigned NaplesPUMCCodeEmitter::
getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {

  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  return 0;
}

void NaplesPUMCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  unsigned Bits = getBinaryCodeForInstr(MI, Fixups, STI);
  EmitLEConstant(Bits, 4, OS);

  ++MCNumEmitted;  // Keep track of the # of mi's emitted.
}

unsigned NaplesPUMCCodeEmitter::encodeABShValue(const MCInst &MI, unsigned Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  // with LEAH the second operand is expected to be the global address
  //MCOperand address = MI.getOperand(1);
  // the third operand is the label used in the asm
  //MCOperand address_asm = MI.getOperand(2);

  if (MI.getOperand(1).isExpr()) {
    Fixups.push_back(MCFixup::create(
        0, MI.getOperand(1).getExpr(),
        MCFixupKind(NaplesPU::fixup_NaplesPU_Absh)));
  } else if ( MI.getOperand(2).isExpr()) {
    Fixups.push_back(MCFixup::create(
        0,  MI.getOperand(2).getExpr(),
        MCFixupKind(NaplesPU::fixup_NaplesPU_Absh)));
  }

  return 0;
}

unsigned NaplesPUMCCodeEmitter::encodeABSlValue(const MCInst &MI, unsigned Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  // with LEAH the third operand is expected to be the global address
  //MCOperand address = MI.getOperand(2);
  // the fourth operand is the label used in the asm
  //MCOperand address_asm = MI.getOperand(3);

  if (MI.getOperand(2).isExpr()) {
    Fixups.push_back(MCFixup::create(
        0, MI.getOperand(2).getExpr(),
        MCFixupKind(NaplesPU::fixup_NaplesPU_Absl)));
  } else if (MI.getOperand(3).isExpr()) {
    Fixups.push_back(MCFixup::create(
        0, MI.getOperand(3).getExpr(),
        MCFixupKind(NaplesPU::fixup_NaplesPU_Absl)));
  }

  return 0;
}

unsigned NaplesPUMCCodeEmitter::encodeLEAValue(const MCInst &MI, unsigned Op,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  MCOperand baseReg = MI.getOperand(1);
  MCOperand offsetOp = MI.getOperand(2);

  assert(baseReg.isReg() && "First operand of LEA op is not register.");
  unsigned encoding = Ctx.getRegisterInfo()->getEncodingValue(baseReg.getReg())<<10;
  if (offsetOp.isExpr()) {
    // Load with a label. This is a PC relative load.  Add a fixup.
    Fixups.push_back(MCFixup::create(
        0, offsetOp.getExpr(),
        MCFixupKind(NaplesPU::fixup_NaplesPU_PCRel_Lea)));
  } else if (offsetOp.isImm())
    encoding |= static_cast<short>(offsetOp.getImm())<<1;
  else
    assert(offsetOp.isImm() && "Second operand of LEA op is unknown type.");

  return encoding;
}

// Encode NaplesPU Memory Operand.  The result is a packed field with the
// register in the low 5 bits and the offset in the remainder.  The instruction
// patterns will put these into the proper part of the instruction
// (NaplesPUInstrFormats.td).
unsigned
NaplesPUMCCodeEmitter::encodeMemoryOpValue(const MCInst &MI, unsigned Op,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  unsigned encoding;

  MCOperand baseReg;
  MCOperand offsetOp;

  baseReg = MI.getOperand(1);
  offsetOp = MI.getOperand(2); 
  
  // Register
  // This is register/offset.  No need for relocation.
  assert(baseReg.isReg() && "First operand is not register.");
  //encoding just the register
  encoding = Ctx.getRegisterInfo()->getEncodingValue(baseReg.getReg());
  // Offset
  if (offsetOp.isExpr()) {
    // Load with a label. This is a PC relative load.  Add a fixup.
    // XXX Note that this assumes unmasked instructions.  A masked
    // instruction will not work and should not be used. Check for this
    // and return an error.
    Fixups.push_back(
        MCFixup::create(0, offsetOp.getExpr(),
                        MCFixupKind(NaplesPU::fixup_NaplesPU_PCRel_Mem)));
  } else if (offsetOp.isImm())
    encoding |= static_cast<short>(offsetOp.getImm()) << 6;
  else
    assert(offsetOp.isImm() && "Second operand of memory op is unknown type.");
  return encoding;
}

// encodeBranchTargetOpValue - Return binary encoding of the jump
// target operand. If the machine operand requires relocation,
// record the relocation and return zero.
unsigned NaplesPUMCCodeEmitter::encodeBranchTargetOpValue(
    const MCInst &MI, unsigned OpNo, SmallVectorImpl<MCFixup> &Fixups,
    const MCSubtargetInfo &STI) const {

  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isImm())
    return MO.getImm();

  assert(MO.isExpr() &&
         "encodeBranchTargetOpValue expects only expressions or an immediate");

  const MCExpr *Expr = MO.getExpr();
  Fixups.push_back(
      MCFixup::create(0, Expr, MCFixupKind(NaplesPU::fixup_NaplesPU_Branch)));
  return 0;
}

#include "NaplesPUGenMCCodeEmitter.inc"
