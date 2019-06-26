//===-- NaplesPURegisterInfo.h - NaplesPU Register Information Impl ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the NaplesPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "NaplesPURegisterInfo.h"
#include "NaplesPU.h"
#include "NaplesPUInstrInfo.h"
#include "NaplesPUSubtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "NaplesPUGenRegisterInfo.inc"

using namespace llvm;

NaplesPURegisterInfo::NaplesPURegisterInfo() : NaplesPUGenRegisterInfo(NaplesPU::FP_REG) {}

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//

const uint16_t *
NaplesPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return NaplesPUCSR_SaveList;
}

const uint32_t *
NaplesPURegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID) const {
  return NaplesPUCSR_RegMask;
}

BitVector NaplesPURegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(NaplesPU::TR_REG);
  Reserved.set(NaplesPU::MR_REG);
  Reserved.set(NaplesPU::FP_REG);
  Reserved.set(NaplesPU::SP_REG);
  Reserved.set(NaplesPU::RA_REG);
  Reserved.set(NaplesPU::PC_REG);
  return Reserved;
}

// Returns a TargetRegisterClass used for pointer values.
const TargetRegisterClass *
NaplesPURegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                      unsigned Kind) const {
  // Use GPR32 RegisterClass to store pointer values.
  return &NaplesPU::GPR32RegClass;
}

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
//TODO: cosi non è limitato lo stack (32 bit = 4 GB).
void NaplesPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
// Inserire istruzioni per la traduzione del frame index.

  assert(SPAdj == 0 && "Unexpected");

  MachineInstr &MI = *II;
  DebugLoc dl = MI.getDebugLoc();
  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  MachineFunction &MF = *MI.getParent()->getParent();
  const TargetFrameLowering &TFL = *MF.getSubtarget().getFrameLowering();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Round stack size to multiple of 64, consistent with frame pointer info.
  int stackSize = alignTo(MFI.getStackSize(), TFL.getStackAlignment());

  // Frame index is relative to where SP is before it is decremented on
  // entry to the function. Need to add stackSize to adjust for this.
  int64_t Offset = MF.getFrameInfo().getObjectOffset(FrameIndex) +
                   MI.getOperand(FIOperandNum + 1).getImm() + stackSize;

  // Determine where callee saved registers live in the frame
  const std::vector<CalleeSavedInfo> &CSI = MFI.getCalleeSavedInfo();
  int MinCSFI = 0;
  int MaxCSFI = -1;
  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }

  // When we save callee saved registers (which includes FP), we must use
  // the SP reg, because FP is not set up yet.
  unsigned FrameReg;
  if (FrameIndex >= MinCSFI && FrameIndex <= MaxCSFI)
    FrameReg = NaplesPU::SP_REG;
  else
    FrameReg = getFrameRegister(MF);

  // Replace frame index with a frame pointer reference.
  if (isInt<9>(Offset)) {
    // If the offset is small enough to fit in the immediate field, directly
    // encode it.
    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
    //TODO: 32 bit troppi, controllare
  } else if (isInt<25>(Offset)) {
	// with 18 bits offset, 256 kB
    MachineBasicBlock &MBB = *II->getParent();
    const NaplesPUInstrInfo &TII =
        *static_cast<const NaplesPUInstrInfo *>(MF.getSubtarget().getInstrInfo());

    MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
    // Crea registro virtuale
    unsigned Reg = RegInfo.createVirtualRegister(&NaplesPU::GPR32RegClass);
    // Inserisci l'immediato nel registro ottenuto in precedenza
    // tramite la MOVEIH e la MOVEIL
    BuildMI(MBB, II, dl, TII.get(NaplesPU::MOVEIHSI), Reg).addImm((Offset >> 16) & 0xffff);
	  BuildMI(MBB, II, dl, TII.get(NaplesPU::MOVEILSI), Reg).addImm(Offset & 0xffff);
    BuildMI(MBB, II, dl, TII.get(NaplesPU::ADDSSS_32), Reg).addReg(FrameReg).addReg(Reg);
    MI.getOperand(FIOperandNum).ChangeToRegister(Reg, false);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(0);
  } else
    report_fatal_error("frame index out of bounds: frame too large");


}

unsigned NaplesPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? NaplesPU::FP_REG : NaplesPU::SP_REG;
}


// We enable the machine register scavenger which provides a mechanism to make
// registers available by evicting them to spill slots.
bool NaplesPURegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool NaplesPURegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

bool NaplesPURegisterInfo::requiresFrameIndexScavenging(const MachineFunction &MF) const {
  return true;
}
