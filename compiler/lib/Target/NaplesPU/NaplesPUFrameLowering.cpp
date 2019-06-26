//===-- NaplesPUFrameLowering.cpp - NaplesPU Frame Information -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the NaplesPU implementation of TargetFrameLowering
// class.
//
//===----------------------------------------------------------------------===//

#include "NaplesPUFrameLowering.h"
#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "NaplesPUInstrInfo.h"
#include "NaplesPUMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

void NaplesPUFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  assert(&MF.front() == &MBB);
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const NaplesPUInstrInfo &TII =
      *static_cast<const NaplesPUInstrInfo *>(MF.getSubtarget().getInstrInfo());
  const MCRegisterInfo *MRI = MF.getMMI().getContext().getRegisterInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();

  // Debug location must be unknown since the first debug location is used
  // to determine the end of the prologue.
  DebugLoc DL;

  // Compute stack size to allocate, keeping SP 64 byte aligned so we
  // can do block vector load/stores
  int StackSize = alignTo(MFI.getStackSize(), getStackAlignment());

  // Bail if there is no stack allocation
  if (StackSize == 0 && !MFI.adjustsStack())
    return;

  TII.adjustStackPointer(MBB, MBBI, DL, -StackSize);

  // Emit DW_CFA_def_cfa
  unsigned CFIIndex = MF.addFrameInst(
      MCCFIInstruction::createDefCfaOffset(nullptr, -StackSize));
  BuildMI(MBB, MBBI, DL, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);

  // Find the instruction past the last instruction that saves a callee-saved
  // register to the stack.  We need to set up FP after its old value has been
  // saved.
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  if (CSI.size()) {
    for (unsigned i = 0; i < CSI.size(); ++i)
      ++MBBI;

    // Iterate over list of callee-saved registers and emit .cfi_offset
    // directives (debug information)
    for (const auto &I : CSI) {
      int64_t Offset = MFI.getObjectOffset(I.getFrameIdx());
      unsigned Reg = I.getReg();
      unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::createOffset(
          nullptr, MRI->getDwarfRegNum(Reg, 1), Offset));
      BuildMI(MBB, MBBI, DL, TII.get(TargetOpcode::CFI_INSTRUCTION))
          .addCFIIndex(CFIIndex);
    }
  }

  // fp = sp
  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, DL, TII.get(NaplesPU::MOVE_SS_32))
        .addReg(NaplesPU::FP_REG)
        .addReg(NaplesPU::SP_REG);

    // emit ".cfi_def_cfa_register $fp" (debug information)
    unsigned CFIIndex = MF.addFrameInst(MCCFIInstruction::createDefCfaRegister(
        nullptr, MRI->getDwarfRegNum(NaplesPU::FP_REG, true)));
    BuildMI(MBB, MBBI, DL, TII.get(TargetOpcode::CFI_INSTRUCTION))
        .addCFIIndex(CFIIndex);
  }
}

void NaplesPUFrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const NaplesPUInstrInfo &TII =
      *static_cast<const NaplesPUInstrInfo *>(MF.getSubtarget().getInstrInfo());
  DebugLoc DL = MBBI->getDebugLoc();
  assert(MBBI->getOpcode() == NaplesPU::JRET &&
         "Can only put epilog before 'retl' instruction!");

  // if framepointer enabled, restore the stack pointer.
  if (hasFP(MF)) {
    // Find the first instruction that restores a callee-saved register.
    MachineBasicBlock::iterator I = MBBI;
    for (unsigned i = 0; i < MFI.getCalleeSavedInfo().size(); ++i)
      --I;

    BuildMI(MBB, I, DL, TII.get(NaplesPU::MOVE_SS_32))
        .addReg(NaplesPU::SP_REG)
        .addReg(NaplesPU::FP_REG);
  }

  uint64_t StackSize = alignTo(MFI.getStackSize(), getStackAlignment());
  if (!StackSize)
    return;

  TII.adjustStackPointer(MBB, MBBI, DL, StackSize);
}

MachineBasicBlock::iterator NaplesPUFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator MBBI) const {
  MachineInstr &MI = *MBBI;

  const NaplesPUInstrInfo &TII =
      *static_cast<const NaplesPUInstrInfo *>(MF.getSubtarget().getInstrInfo());

  // Note the check for hasReservedCallFrame.  If it returns true,
  // PEI::calculateFrameObjectOffsets has already reserved stack locations for
  // these variables and we don't need to adjust the stack here.
  int Amount = TII.getFrameSize(MI);
  if (Amount != 0 && !hasReservedCallFrame(MF)) {
    assert(hasFP(MF) &&
           "Cannot adjust stack mid-function without a frame pointer");

    if (MI.getOpcode() == NaplesPU::ADJCALLSTACKDOWN)
      Amount = -Amount;

    TII.adjustStackPointer(MBB, MBBI, MBBI->getDebugLoc(), Amount);
  }

  return MBB.erase(MBBI);
}

void NaplesPUFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                              BitVector &SavedRegs,
                                              RegScavenger *RS) const {

  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  if (hasFP(MF))
    SavedRegs.set(NaplesPU::FP_REG);

  // The register scavenger allows us to allocate virtual registers during
  // epilogue/prologue insertion, after register allocation has run. We only
  // need to do this if the frame is too large to be addressed by immediate
  // offsets. If it isn't, don't bother creating a stack slot for it.
  // This may create the scavenge slot when it isn't needed.
  // This check must match the size of the one in
  // NaplesPURegisterInfo::eliminateFrameIndex
  if (isInt<9>(getWorstCaseStackSize(MF)))
    return;

  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  const TargetRegisterClass &RC = NaplesPU::GPR32RegClass;
  int FI = MF.getFrameInfo().CreateStackObject(TRI->getSpillSize(RC),
                                               TRI->getSpillAlignment(RC), false);
  RS->addScavengingFrameIndex(FI);
}

// We must use an FP in a few situations.  Note that this *must* return true if
// hasReservedCallFrame returns false.  Otherwise an ADJCALLSTACKDOWN could mess
// up frame offsets from the stack pointer.
bool NaplesPUFrameLowering::hasFP(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MFI.hasVarSizedObjects() || MFI.isFrameAddressTaken();
}

// Returns true if the prologue inserter should reserve space for outgoing
// arguments to called.
bool NaplesPUFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  return !MF.getFrameInfo().hasVarSizedObjects();
}

uint64_t
NaplesPUFrameLowering::getWorstCaseStackSize(const MachineFunction &MF) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetRegisterInfo &TRI = *MF.getSubtarget().getRegisterInfo();

  int64_t Offset = 0;

  // Iterate over fixed sized objects.
  for (int I = MFI.getObjectIndexBegin(); I != 0; ++I)
    Offset = std::max(Offset, -MFI.getObjectOffset(I));

  // Conservatively assume all callee-saved registers will be saved.
  for (const uint16_t *R = TRI.getCalleeSavedRegs(&MF); *R; ++R) {
    const TargetRegisterClass *RC = TRI.getMinimalPhysRegClass(*R);
    unsigned Size = TRI.getRegSizeInBits(*RC);
    Offset = alignTo(Offset + Size, Size);
  }

  unsigned MaxAlign = MFI.getMaxAlignment();

  // Check that MaxAlign is not zero if there is a stack object that is not a
  // callee-saved spill.
  assert(!MFI.getObjectIndexEnd() || MaxAlign);

  // Iterate over other objects.
  for (unsigned I = 0, E = MFI.getObjectIndexEnd(); I != E; ++I)
    Offset = alignTo(Offset + MFI.getObjectSize(I), MaxAlign);

  // Call frame.
  if (MFI.adjustsStack() && hasReservedCallFrame(MF))
    Offset = alignTo(Offset + MFI.getMaxCallFrameSize(),
                     std::max(MaxAlign, getStackAlignment()));

  return alignTo(Offset, getStackAlignment());
}
