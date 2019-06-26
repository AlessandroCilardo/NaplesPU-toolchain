//===-- NaplesPUMCTargetDesc.h - NaplesPU Target Descriptions ---------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPUMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
class Target;
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class raw_ostream;
class StringRef;
class Triple;

extern Target TheNaplesPUTarget;

MCCodeEmitter *createNaplesPUMCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);

std::unique_ptr<MCObjectTargetWriter> createNaplesPUELFObjectWriter(uint8_t OSABI);

MCAsmBackend *createNaplesPUAsmBackend(const Target &T,
                                    const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

} // namespace llvm

// Defines symbolic names for NaplesPU registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "NaplesPUGenRegisterInfo.inc"

// Defines symbolic names for the NaplesPU instructions.
//
#define GET_INSTRINFO_ENUM
#include "NaplesPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "NaplesPUGenSubtargetInfo.inc"

#endif
