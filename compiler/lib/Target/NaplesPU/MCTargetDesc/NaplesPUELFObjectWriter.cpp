//===-- NaplesPUELFObjectWriter.cpp - NaplesPU ELF Writer ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/NaplesPUFixupKinds.h"
#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class NaplesPUELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit NaplesPUELFObjectWriter(uint8_t OSABI);

protected:
  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override;
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // namespace

NaplesPUELFObjectWriter::NaplesPUELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, ELF::EM_NPU,
                              /*HasRelocationAddend*/ true) {}

bool NaplesPUELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
                             unsigned Type) const
{
  return false;
}

unsigned NaplesPUELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  unsigned Type;
  unsigned Kind = (unsigned)Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Invalid fixup kind!");
  case FK_Data_4:
    Type = ELF::R_NPU_ABS32;
    break;

  case NaplesPU::fixup_NaplesPU_Branch:
    Type = ELF::R_NPU_BRANCH;
    break;

  case NaplesPU::fixup_NaplesPU_PCRel_Mem:
   // assert(IsPCRel);
    Type = ELF::R_NPU_PCREL_MEM;
    break;
  
  case NaplesPU::fixup_NaplesPU_PCRel_Lea:
    //assert(IsPCRel);
    Type = ELF::R_NPU_PCREL_LEA;
    break;
  case NaplesPU::fixup_NaplesPU_PCRel_Ext:
    //assert(IsPCRel);
    Type = ELF::R_NPU_PCREL_EXT;
    break;
  case NaplesPU::fixup_NaplesPU_Absh:
  //  assert(IsPCRel);
    Type = ELF::R_NPU_ABS_HIGH_LEA;
    break;
  case NaplesPU::fixup_NaplesPU_Absl:
//    assert(!IsPCRel);
    Type = ELF::R_NPU_ABS_LOW_LEA;
    break;
  }
  return Type;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createNaplesPUELFObjectWriter(uint8_t OSABI) {
  return llvm::make_unique<NaplesPUELFObjectWriter>(OSABI);
}
