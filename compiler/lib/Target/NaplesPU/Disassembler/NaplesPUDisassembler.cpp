//===- NaplesPUDisassembler.cpp - Disassembler for NaplesPU ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the NaplesPU Disassembler.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "naplespu-disassembler"

#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "NaplesPU.h"
#include "NaplesPURegisterInfo.h"
#include "NaplesPUSubtarget.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace llvm {

class NaplesPUDisassembler : public MCDisassembler {
public:
  NaplesPUDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx,
                    const MCRegisterInfo *Info)
      : MCDisassembler(STI, Ctx), RegInfo(Info) {}

  //~NaplesPUDisassembler();

  const MCRegisterInfo *getRegInfo() const { return RegInfo; }

  /// getInstruction - See MCDisassembler.
  /// Returns the disassembly of a single instruction.
  ///
  /// \param Instr    - An MCInst to populate with the contents of the
  ///                   instruction.
  /// \param Size     - A value to populate with the size of the instruction, or
  ///                   the number of bytes consumed while attempting to decode
  ///                   an invalid instruction.
  /// \param Address  - The address, in the memory space of region, of the first
  ///                   byte of the instruction.
  /// \param VStream  - The stream to print warnings and diagnostic messages on.
  /// \param CStream  - The stream to print comments and annotations on.
  /// \return         - MCDisassembler::Success if the instruction is valid,
  ///                   MCDisassembler::SoftFail if the instruction was
  ///                                            disassemblable but invalid,
  ///                   MCDisassembler::Fail if the instruction was invalid.
  DecodeStatus getInstruction(MCInst &instr, uint64_t &size,
                              ArrayRef<uint8_t> Bytes, uint64_t address,
                              raw_ostream &vStream,
                              raw_ostream &cStream) const override;

private:
  const MCRegisterInfo *RegInfo;
};

} // namespace llvm


static DecodeStatus decodeSimm9Value(MCInst &Inst, unsigned Insn,
                                     uint64_t Address, const void *Decoder);

static DecodeStatus decodeSimm16Value(MCInst &Inst, unsigned Insn,
                                      uint64_t Address, const void *Decoder);

static DecodeStatus decodeScalarMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder);

static DecodeStatus decodeVectorWMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder);

static DecodeStatus decodeVectorLMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder);

static DecodeStatus decodeBranchTargetOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder);

DecodeStatus DecodeGPR32RegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder);

/*DecodeStatus DecodeGPR64RegisterClass(MCInst &Inst, unsigned RegNo,
                                     uint64_t Address, const void *Decoder);
*/
DecodeStatus DecodeVR512WRegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder);

/*DecodeStatus DecodeVR512LRegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder);
*/
namespace llvm {
extern Target TheNaplesPUTarget;
}

static MCDisassembler *createNaplesPUDisassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new NaplesPUDisassembler(STI, Ctx, T.createMCRegInfo(""));
}

extern "C" void LLVMInitializeNaplesPUDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheNaplesPUTarget,
                                         createNaplesPUDisassembler);
}

#include "NaplesPUGenDisassemblerTables.inc"

/// readInstruction - read four bytes from the MemoryObject
/// and return 32 bit word sorted according to the given endianess
static DecodeStatus readInstruction32(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint32_t &Insn) {
  // We want to read exactly 4 Bytes of data.
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn =
      (Bytes[0] << 0) | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);

  return MCDisassembler::Success;
}

//NaplesPUDisassembler::~NaplesPUDisassembler() {}

DecodeStatus NaplesPUDisassembler::getInstruction(MCInst &instr, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &vStream,
                                               raw_ostream &cStream) const {
  uint32_t Insn;

  DecodeStatus Result = readInstruction32(Bytes, Address, Size, Insn);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Calling the auto-generated decoder function.
  Result = decodeInstruction(DecoderTable32, instr, Insn, Address, this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  return MCDisassembler::Fail;
}

static unsigned getReg(const void *D, unsigned RC, unsigned RegNo) {
  const NaplesPUDisassembler *Dis = static_cast<const NaplesPUDisassembler *>(D);
  return *(Dis->getRegInfo()->getRegClass(RC).begin() + RegNo);
}

static DecodeStatus decodeSimm9Value(MCInst &Inst, unsigned Insn,
                                     uint64_t Address, const void *Decoder) {

  Inst.addOperand(MCOperand::createImm(SignExtend32<9>(Insn)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeSimm16Value(MCInst &Inst, unsigned Insn,
                                      uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::createImm(SignExtend32<16>(Insn)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMemoryOpValue(MCInst &Inst, unsigned Insn,
                                        uint64_t Address, const void *Decoder,
                                        unsigned RC) {
  // Offset as defined in encodeMemoryOpValue bits from 6 to 14
  int Offset = SignExtend32<9>(fieldFromInstruction(Insn, 6, 9));
  // RegisterIndex as defined in encodeMemoryOpValue bits from 0 to 5
  int RegisterIndex = fieldFromInstruction(Insn, 0, 6);
  unsigned BaseReg = getReg(Decoder, RC, RegisterIndex);

  Inst.addOperand(MCOperand::createReg(BaseReg));
  Inst.addOperand(MCOperand::createImm(Offset));

  return MCDisassembler::Success;
}

static DecodeStatus decodeScalarMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder) {
  return decodeMemoryOpValue(Inst, Insn, Address, Decoder,
                             NaplesPU::GPR32RegClassID);
}

static DecodeStatus decodeVectorWMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder) {
  return decodeMemoryOpValue(Inst, Insn, Address, Decoder,
                             NaplesPU::VR512WRegClassID);
}
/*
static DecodeStatus decodeVectorLMemoryOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder) {
  return decodeMemoryOpValue(Inst, Insn, Address, Decoder,
                             NaplesPU::VR512LRegClassID);
}*/

//TODO: MANCA IL FORMATO J
static DecodeStatus decodeBranchTargetOpValue(MCInst &Inst, unsigned Insn,
                                              uint64_t Address,
                                              const void *Decoder) {
  const MCDisassembler *Dis = static_cast<const MCDisassembler *>(Decoder);
  if (!Dis->tryAddingSymbolicOperand(Inst, Address + 4 + SignExtend32<18>(Insn),
                                     Address, true, 0, 4)) {
    Inst.addOperand(MCOperand::createImm(SignExtend32<18>(Insn)));
  }

  return MCDisassembler::Success;
}

//---------- Decode Register class ----------//
DecodeStatus DecodeGPR32RegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder) {

  if (RegNo > 63)
    return MCDisassembler::Fail;

  // The internal representation of the registers counts r0: 1, r1: 2, etc.
  Inst.addOperand(
      MCOperand::createReg(getReg(Decoder, NaplesPU::GPR32RegClassID, RegNo)));
  return MCDisassembler::Success;
}

/*DecodeStatus DecodeGPR64RegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder) {

  if (RegNo > 63 || RegNo%2==1)
    return MCDisassembler::Fail;

  // The internal representation of the registers counts r0: 1, r1: 2, etc.
  Inst.addOperand(
      MCOperand::createReg(getReg(Decoder, NaplesPU::GPR64RegClassID, RegNo/2)));
  return MCDisassembler::Success;
}*/


DecodeStatus DecodeVR512WRegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder) {

  if (RegNo > 63)
    return MCDisassembler::Fail;

  // The internal representation of the registers counts r0: 1, r1: 2, etc.
  Inst.addOperand(
      MCOperand::createReg(getReg(Decoder, NaplesPU::VR512WRegClassID, RegNo)));
  return MCDisassembler::Success;
}
/*
DecodeStatus DecodeVR512LRegisterClass(MCInst &Inst, unsigned RegNo,
                                      uint64_t Address, const void *Decoder) {

  if (RegNo > 63)
    return MCDisassembler::Fail;

  // The internal representation of the registers counts r0: 1, r1: 2, etc.
  Inst.addOperand(
      MCOperand::createReg(getReg(Decoder, NaplesPU::VR512LRegClassID, RegNo)));
  return MCDisassembler::Success;
}*/
