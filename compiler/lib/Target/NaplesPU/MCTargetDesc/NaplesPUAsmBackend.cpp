//===-- NaplesPUAsmBackend.cpp - NaplesPU Assembler Backend ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "NaplesPUFixupKinds.h"
#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;


namespace {
  class NaplesPUAsmBackend : public MCAsmBackend {
  Triple::OSType OSType;

  public:
NaplesPUAsmBackend(const Target &T, Triple::OSType _OSType)
          : MCAsmBackend(support::little), OSType(_OSType) {}

//UNUSED, these PCRel istructions read PC and not PC+4.
unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                             MCContext *Ctx) const {  
 switch ((unsigned int)Fixup.getKind()) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
  case NaplesPU::fixup_NaplesPU_Branch:
  case NaplesPU::fixup_NaplesPU_PCRel_Mem:
  case NaplesPU::fixup_NaplesPU_PCRel_Lea:
  case NaplesPU::fixup_NaplesPU_PCRel_Ext:
  case NaplesPU::fixup_NaplesPU_Absl:
  case NaplesPU::fixup_NaplesPU_Absh:
    //Value -= 4; // source location is PC + 4
    break;
  }
  return Value;
}
void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                   const MCValue &Target, MutableArrayRef<char> Data,
                   uint64_t Value, bool IsResolved,
                   const MCSubtargetInfo*) const override {
     const MCFixupKindInfo &Info = getFixupKindInfo(Fixup.getKind());
     Value = adjustFixupValue(Fixup, Value, &Asm.getContext());
     unsigned Offset = Fixup.getOffset();
     unsigned NumBytes = 4;
 
     uint64_t CurVal = 0;
     for (unsigned i = 0; i != NumBytes; ++i) {
       CurVal |= (uint64_t)((uint8_t)Data[Offset + i]) << (i * 8);
     }
 
     uint64_t Mask = ((uint64_t)(-1) >> (64 - Info.TargetSize));
 
     Value <<= Info.TargetOffset;
     Mask <<= Info.TargetOffset;
     CurVal |= Value & Mask;
 
     // Write out the fixed up bytes back to the code/data bits.
     for (unsigned i = 0; i != NumBytes; ++i) {
       Data[Offset + i] = (uint8_t)((CurVal >> (i * 8)) & 0xff);
     }
   }

   std::unique_ptr<MCObjectTargetWriter>
   createObjectTargetWriter() const override {
     return createNaplesPUELFObjectWriter(
         MCELFObjectTargetWriter::getOSABI(OSType));
   }

    unsigned getNumFixupKinds() const {
      return NaplesPU::NumTargetFixupKinds;
    }

    const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
      const static MCFixupKindInfo Infos[NaplesPU::NumTargetFixupKinds] = {
        // This table *must* be in same the order of fixup_* kinds in
        // NaplesPUFixupKinds.h.
        //
        // name                          offset  bits  flags
        {"fixup_NaplesPU_Branch", 0, 18, MCFixupKindInfo::FKF_IsPCRel},
        {"fixup_NaplesPU_PCRel_Mem", 3, 9, MCFixupKindInfo::FKF_IsPCRel},
        {"fixup_NaplesPU_PCRel_Lea", 3, 9, MCFixupKindInfo::FKF_IsPCRel},
        {"fixup_NaplesPU_PCRel_Ext", 3, 9, MCFixupKindInfo::FKF_IsPCRel}
      };

      if (Kind < FirstTargetFixupKind)
        return MCAsmBackend::getFixupKindInfo(Kind);

      assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
             "Invalid kind!");
      return Infos[Kind - FirstTargetFixupKind];
    }

 bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override {
     return false;
   }

    /// fixupNeedsRelaxation - Target specific predicate for whether a given
    /// fixup requires the associated instruction to be relaxed.
    bool fixupNeedsRelaxation(const MCFixup &Fixup,
                              uint64_t Value,
                              const MCRelaxableFragment *DF,
                              const MCAsmLayout &Layout) const override {
      return false;
    }
    void relaxInstruction(const MCInst &Inst, const MCSubtargetInfo &STI,
                          MCInst &Res) const override {
      assert(0 && "relaxInstruction() unimplemented");
    }

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override {
     OS.write_zeros(Count);
     return true;
   }

  };

} // end anonymous namespace
MCAsmBackend *llvm::createNaplesPUAsmBackend(const Target &T,
                                           const MCSubtargetInfo &STI,
                                           const MCRegisterInfo &MRI,
                                           const MCTargetOptions &Options) {
   return new NaplesPUAsmBackend(T, STI.getTargetTriple().getOS());
 }

