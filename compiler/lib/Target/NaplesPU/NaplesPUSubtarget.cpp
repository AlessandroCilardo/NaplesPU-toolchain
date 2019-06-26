//===-- NaplesPUSubtarget.cpp - NaplesPU Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the NaplesPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "NaplesPU.h"
#include "NaplesPUSubtarget.h"
#include "NaplesPUTargetMachine.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "NaplesPU-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "NaplesPUGenSubtargetInfo.inc"

void NaplesPUSubtarget::anchor() { }

NaplesPUSubtarget::NaplesPUSubtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS, const NaplesPUTargetMachine &TM)
    : NaplesPUGenSubtargetInfo(TT, CPU, FS),
      InstrInfo(NaplesPUInstrInfo::create(*this)),
      TLInfo(NaplesPUTargetLowering::create(TM, *this)), TSInfo(),
      FrameLowering((*this)) {

      // Determine default and user specified characteristics
      std::string CPUName = CPU;
      // CPUName is empty when invoked from tools like llc
      if (CPUName.empty())
      CPUName = "naplespu";

      // Parse features string.
      ParseSubtargetFeatures(CPUName, FS);
}

/*
//getAdjustedFrameSize ed enableMachineScheduler commentati

int NaplesPUSubtarget::getAdjustedFrameSize(int frameSize) const {

  if (is64Bit()) {
    // All 64-bit stack frames must be 16-byte aligned, and must reserve space
    // for spilling the 16 window registers at %sp+BIAS..%sp+BIAS+128.
    frameSize += 128;
    // Frames with calls must also reserve space for 6 outgoing arguments
    // whether they are used or not. LowerCall_64 takes care of that.
    frameSize = RoundUpToAlignment(frameSize, 16);
  } else {
    // Emit the correct save instruction based on the number of bytes in
    // the frame. Minimum stack frame size according to V8 ABI is:
    //   16 words for register window spill
    //    1 word for address of returned aggregate-value
    // +  6 words for passing parameters on the stack
    // ----------
    //   23 words * 4 bytes per word = 92 bytes
    frameSize += 92;

    // Round up to next doubleword boundary -- a double-word boundary
    // is required by the ABI.
    frameSize = RoundUpToAlignment(frameSize, 8);
  }
  return frameSize;
}


bool NaplesPUSubtarget::enableMachineScheduler() const {
  return true;
}
*/
