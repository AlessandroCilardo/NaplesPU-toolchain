//===-- NaplesPUMCAsmInfo.cpp - NaplesPU asm properties ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the NaplesPUMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "NaplesPUMCAsmInfo.h"
#include "llvm/ADT/Triple.h"

using namespace llvm;

void NaplesPUMCAsmInfo::anchor() {}

NaplesPUMCAsmInfo::NaplesPUMCAsmInfo(const Triple &TT) {
  IsLittleEndian = true;
  SupportsDebugInformation = true;

  // Even though this target doesn't support exceptions, this must be enabled
  // to generate the call frame information (even .debug_frame is not generated
  // if this is not set.  See emitCFIInstruction,
  // CodeGen/AsmPrinter/AsmPrinter.cpp).
  ExceptionsType = ExceptionHandling::DwarfCFI;

  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
}
