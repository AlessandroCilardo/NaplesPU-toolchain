//===-- NaplesPUFixupKinds.h - NaplesPU Specific Fixup Entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_MCTARGETDESC_NAPLESPUFIXUPKINDS_H
#define LLVM_LIB_TARGET_NAPLESPU_MCTARGETDESC_NAPLESPUFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
  namespace NaplesPU {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.
  //
  // This table *must* be in the same order of
  // MCFixupKindInfo Infos[NaplesPU::NumTargetFixupKinds]
  // in NaplesPUAsmBackend.cpp.
    enum Fixups {
	
	// PC relative offset for extended memory access
        fixup_NaplesPU_Branch = FirstTargetFixupKind, // PC relative offset for extended memory access
        fixup_NaplesPU_PCRel_Mem,              // PC relative offset for memory access
        fixup_NaplesPU_PCRel_Lea,              // PC relative for branch instruction
        fixup_NaplesPU_PCRel_Ext,
        fixup_NaplesPU_Absh,
        fixup_NaplesPU_Absl,

      // Marker
      LastTargetFixupKind,
      NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
    };
  }
}

#endif
