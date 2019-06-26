
//===- NaplesPU.cpp ----------------------------------------------------------===//
//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "SyntheticSections.h"
#include "Target.h"
#include "Thunks.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {

class NaplesPU final : public TargetInfo {
public:
  NaplesPU();
  void relocateOne(uint8_t *Loc, uint32_t Type, uint64_t Val) const override;
  RelExpr getRelExpr(RelType Type, const Symbol &S,
                     const uint8_t *Loc) const override;
  RelType getDynRel(RelType Type) const override;
};

template <uint8_t SIZE, uint8_t BASE>
void applyNaplesPUReloc(uint8_t *Loc, uint32_t Type, uint64_t V) {
  uint32_t Mask = (0xffffffff >> (32 - SIZE)) << BASE;
  write32le(Loc, (read32le(Loc) & ~Mask) | ((V << BASE) & Mask));
}
} // namespace

NaplesPU::NaplesPU() {
  PageSize = 0x1000;
  DefaultImageBase = 0;

  GotBaseSymInGotPlt = false;
  GotRel = R_NPU_ABS32;
  PltRel = -1;
  TlsDescRel = -1;
  TlsGotRel = -1;
  GotEntrySize = 4;
  GotPltEntrySize = 4;
  PltEntrySize = 16;
  PltHeaderSize = 32;
}

void NaplesPU::relocateOne(uint8_t *Loc, uint32_t Type, uint64_t Val) const {
  int64_t Offset;
  switch (Type) {
  default:
    fatal("unrecognized reloc " + Twine(Type));
    case R_NPU_ABS32:
     write32le(Loc, Val);
     break;
   case R_NPU_BRANCH:
     //Check Offset for J-Type Instructions
     checkInt(Loc, Val, 18, Type);
     applyNaplesPUReloc<18, 0>(Loc, Type, Val);
     break;
   case R_NPU_PCREL_MEM:
     //Check Offset for M-Type Instructions
     checkInt(Loc, Val, 9, Type);
     applyNaplesPUReloc<9, 3>(Loc, Type, Val);
     break;
   case R_NPU_PCREL_LEA:
     //Check Offset for I-Type Instructions
     checkInt(Loc, Val, 9, Type);
     applyNaplesPUReloc<9, 3>(Loc, Type, Val);
     break;
   case R_NPU_PCREL_EXT:
     checkInt(Loc, Val, 9, Type);
     applyNaplesPUReloc<9,3>(Loc, Type, Val);
     break;
   case R_NPU_ABS_HIGH_LEA:
     //Check Offset for MOVEI-Type Instructions
     checkUInt(Loc, (Val >> 16) & 0xFFFF , 16, Type);
     applyNaplesPUReloc<16, 2>(Loc, Type, (Val >> 16) & 0xFFFF);
     break;
   case R_NPU_ABS_LOW_LEA:
     //Check Offset for MOVEI-Type Instructions
     checkUInt(Loc, (Val) & 0xFFFF , 16, Type);
     applyNaplesPUReloc<16, 2>(Loc, Type, (Val) & 0xFFFF);
     break;
  }
}

RelExpr NaplesPU::getRelExpr(RelType Type, const Symbol&,
                          const uint8_t*) const {
  switch (Type) {
  default:
    fatal("unrecognized reloc " + Twine(Type));
  case R_NPU_ABS32:
  case R_NPU_ABS_HIGH_LEA:
  case R_NPU_ABS_LOW_LEA:
    return R_ABS;

  case R_NPU_BRANCH:
    return R_PC;

  case R_NPU_PCREL_MEM:
  case R_NPU_PCREL_LEA:
  case R_NPU_PCREL_EXT:
    return R_GOT;
  }
}

RelType NaplesPU::getDynRel(RelType Type) const {
  if (Type == R_NPU_ABS32)
      return Type;

  return R_NPU_NONE;
}

TargetInfo *elf::getNaplesPUTargetInfo() {
  static NaplesPU Target;
  return &Target;
}
