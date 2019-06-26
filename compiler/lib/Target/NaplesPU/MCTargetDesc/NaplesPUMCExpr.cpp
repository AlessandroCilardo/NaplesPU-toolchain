//===- NaplesPUMCExpr.cpp - NaplesPU specific MC expression classes ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "NaplesPUMCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

#define DEBUG_TYPE "NaplesPUmcexpr"

const NaplesPUMCExpr *NaplesPUMCExpr::create(VariantKind Kind, const MCExpr *Expr,
                                       MCContext &Ctx) {
  return new (Ctx) NaplesPUMCExpr(Kind, Expr);
}

void NaplesPUMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid kind!");
  case VK_NaplesPU_GOT:
    OS << "got";
    break;
  case VK_NaplesPU_ABS_HI:
    OS << "hi";
    break;
  case VK_NaplesPU_ABS_LO:
    OS << "lo";
    break;
  }

  OS << '(';
  const MCExpr *Expr = getSubExpr();
  Expr->print(OS, MAI);
  OS << ')';
}

void NaplesPUMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

bool NaplesPUMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                            const MCAsmLayout *Layout,
                                            const MCFixup *Fixup) const {
  if (!getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup))
    return false;

  Res =
      MCValue::get(Res.getSymA(), Res.getSymB(), Res.getConstant(), getKind());

  return true;
}
