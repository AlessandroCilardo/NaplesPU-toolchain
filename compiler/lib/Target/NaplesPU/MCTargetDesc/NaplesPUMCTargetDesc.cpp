//===-- NaplesPUMCTargetDesc.cpp - NaplesPU Target Descriptions  ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides NaplesPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "NaplesPUMCTargetDesc.h"
#include "InstPrinter/NaplesPUInstPrinter.h"
#include "NaplesPUMCAsmInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "NaplesPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "NaplesPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "NaplesPUGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createNaplesPUMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitNaplesPUMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createNaplesPUMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitNaplesPUMCRegisterInfo(X, NaplesPU::RA_REG);
  return X;
}

static MCSubtargetInfo *
createNaplesPUMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createNaplesPUMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCAsmInfo *createNaplesPUMCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT) {
  MCAsmInfo *MAI = new NaplesPUMCAsmInfo(TT);

  // Put an instruction into the common information entry (CIE), shared
  // by all frame description entries (FDE), which indicates the stack
  // pointer register (r30) is used to find the canonical frame address (CFA).
  unsigned SP = MRI.getDwarfRegNum(NaplesPU::SP_REG, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCStreamer *createNaplesPUMCStreamer(const Triple &T, MCContext &Context,
                                         std::unique_ptr<MCAsmBackend> &&MAB,
                                         std::unique_ptr<MCObjectWriter> &&OW,
                                         std::unique_ptr<MCCodeEmitter> &&Emitter,
                                         bool RelaxAll) {
  return createELFStreamer(Context, std::move(MAB), std::move(OW),
                           std::move(Emitter), RelaxAll);
}

static MCInstPrinter *createNaplesPUMCInstPrinter(const Triple &T,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new NaplesPUInstPrinter(MAI, MII, MRI);
}

namespace {

class NaplesPUMCInstrAnalysis : public MCInstrAnalysis {
public:
  explicit NaplesPUMCInstrAnalysis(const MCInstrInfo *Info) : MCInstrAnalysis(Info) {}

  bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size,
                      uint64_t &Target) const override {

    unsigned NumOps = Inst.getNumOperands();
    if (NumOps == 0)
      return false;
    switch (Info->get(Inst.getOpcode()).OpInfo[NumOps - 1].OperandType) {
    case MCOI::OPERAND_UNKNOWN:
    case MCOI::OPERAND_IMMEDIATE:
      Target = Addr + Inst.getOperand(NumOps - 1).getImm();
      return true;
    default:
      return false;
    }
  }
};

} // namespace

static MCInstrAnalysis *createNaplesPUMCInstrAnalysis(const MCInstrInfo *Info) {
  return new NaplesPUMCInstrAnalysis(Info);
}

extern "C" void LLVMInitializeNaplesPUTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn A(TheNaplesPUTarget, createNaplesPUMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheNaplesPUTarget, createNaplesPUMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheNaplesPUTarget, createNaplesPUMCRegisterInfo);

  TargetRegistry::RegisterMCCodeEmitter(TheNaplesPUTarget,
                                        createNaplesPUMCCodeEmitter);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheNaplesPUTarget,
                                          createNaplesPUMCSubtargetInfo);

  // Register the MC instruction analyzer.
  TargetRegistry::RegisterMCInstrAnalysis(TheNaplesPUTarget,
                                          createNaplesPUMCInstrAnalysis);

  // Register the ASM Backend
  TargetRegistry::RegisterMCAsmBackend(TheNaplesPUTarget, createNaplesPUAsmBackend);

  // Register the object streamer
  TargetRegistry::RegisterELFStreamer(TheNaplesPUTarget, createNaplesPUMCStreamer);

  // MC instruction printer
  TargetRegistry::RegisterMCInstPrinter(TheNaplesPUTarget,
                                        createNaplesPUMCInstPrinter);
}
