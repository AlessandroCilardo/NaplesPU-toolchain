//===-- NaplesPUAsmParser.cpp - Parse NaplesPU assembly to MCInst instructions -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/NaplesPUMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

namespace {
struct NaplesPUOperand;

class NaplesPUAsmParser : public MCTargetAsmParser {
  MCAsmParser &Parser;
  MCAsmParser &getParser() const { return Parser; }
  MCAsmLexer &getLexer() const { return Parser.getLexer(); }

  /// MatchAndEmitInstruction - Recognize a series of operands of a parsed
  /// instruction as an actual MCInst and emit it to the specified MCStreamer.
  /// This returns false on success and returns true on failure to match.
  ///
  /// On failure, the target parser is responsible for emitting a diagnostic
  /// explaining the match failure.
  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;

  /// ParseInstruction - Parse one assembly instruction.
  ///
  /// The parser is positioned following the instruction name. The target
  /// specific instruction parser should parse the entire instruction and
  /// construct the appropriate MCInst, or emit an error. On success, the entire
  /// line should be parsed up to and including the end-of-statement token. On
  /// failure, the parser is not required to read to the end of the line.
  //
  /// \param Name - The instruction name.
  /// \param NameLoc - The source location of the name.
  /// \param Operands [out] - The list of parsed operands, this returns
  ///        ownership of them to the caller.
  /// \return True on failure.
  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;

  /// ParseDirective - Parse a target specific assembler directive
  ///
  /// The parser is positioned following the directive name.  The target
  /// specific directive parser should parse the entire directive doing or
  /// recording any target specific work, or return true and do nothing if the
  /// directive is not target specific. If the directive is specific for
  /// the target, the entire line is parsed up to and including the
  /// end-of-statement token and false is returned.
  ///
  /// \param DirectiveID - the identifier token of the directive.
  bool ParseDirective(AsmToken DirectiveID) override;

  bool ParseImmediate(OperandVector &Ops);

  bool ParseOperand(OperandVector &Operands, StringRef Name);

  bool ProcessInstruction(MCInst &Inst, const SMLoc &Loc, MCStreamer &Out);

// Auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "NaplesPUGenAsmMatcher.inc"

  OperandMatchResultTy ParseMemoryOperand(OperandVector &Operands);

public:
  NaplesPUAsmParser(const MCSubtargetInfo &sti, MCAsmParser &_Parser,
                   const MCInstrInfo &MII, const MCTargetOptions &Options)
        : MCTargetAsmParser(Options, sti, MII), Parser(_Parser) {
      setAvailableFeatures(ComputeAvailableFeatures(sti.getFeatureBits()));
 }
};

/// NaplesPUOperand - Instances of this class represented a parsed machine
/// instruction
/// MCParsedAsmOperand - This abstract class represents a source-level assembly
/// instruction operand.  It should be subclassed by target-specific code.  This
/// base class is used by target-independent clients and is the interface
/// between parsing an asm instruction and recognizing it.
struct NaplesPUOperand : public MCParsedAsmOperand {

  enum KindTy { Token, Register, Immediate, Memory } Kind;

  struct Token {
    const char *Data;
    unsigned Length;
  };

  struct RegisterIndex {
    unsigned RegNum;
  };

  struct ImmediateOperand {
    const MCExpr *Val;
  };

  struct MemoryOperand {
    unsigned BaseReg;
    const MCExpr *Off;
  };

  SMLoc StartLoc, EndLoc;

  union {
    struct Token Tok;
    struct RegisterIndex Reg;
    struct ImmediateOperand Imm;
    struct MemoryOperand Mem;
  };

  NaplesPUOperand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

public:
  NaplesPUOperand(const NaplesPUOperand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
    case Register:
      Reg = o.Reg;
      break;
    case Immediate:
      Imm = o.Imm;
      break;
    case Token:
      Tok = o.Tok;
      break;
    case Memory:
      Mem = o.Mem;
      break;
    }
  }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const { return StartLoc; }

  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const { return EndLoc; }

  unsigned getReg() const {
    assert(Kind == Register && "Invalid type access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == Immediate && "Invalid type access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == Token && "Invalid type access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  unsigned getMemBase() const {
    assert((Kind == Memory) && "Invalid access!");
    return Mem.BaseReg;
  }

  const MCExpr *getMemOff() const {
    assert((Kind == Memory) && "Invalid access!");
    return Mem.Off;
  }

  // Functions for testing operand type
  bool isReg() const { return Kind == Register; }
  bool isImm() const { return Kind == Immediate; }
  bool isToken() const { return Kind == Token; }
  bool isMem() const { return Kind == Memory; }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediates where possible. Null MCExpr = 0
    if (Expr == 0)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  void addMemOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");

    Inst.addOperand(MCOperand::createReg(getMemBase()));
    const MCExpr *Expr = getMemOff();
    addExpr(Inst, Expr);
  }

  void print(raw_ostream &OS) const {
    switch (Kind) {
    case Token:
      OS << "Tok ";
      OS.write(Tok.Data, Tok.Length);
      break;
    case Register:
      OS << "Reg " << Reg.RegNum;
      break;
    case Immediate:
      OS << "Imm ";
      OS << *Imm.Val;
      break;
    case Memory:
      OS << "Mem " << Mem.BaseReg << " ";
      if (Mem.Off)
        OS << *Mem.Off;
      else
        OS << "0";

      break;
    }
  }

  static std::unique_ptr<NaplesPUOperand> createToken(StringRef Str, SMLoc S) {
    auto Op = make_unique<NaplesPUOperand>(Token);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<NaplesPUOperand> createReg(unsigned RegNo, SMLoc S,
                                                 SMLoc E) {
    auto Op = make_unique<NaplesPUOperand>(Register);
    Op->Reg.RegNum = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<NaplesPUOperand> createImm(const MCExpr *Val, SMLoc S,
                                                 SMLoc E) {
    auto Op = make_unique<NaplesPUOperand>(Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<NaplesPUOperand>
  createMem(unsigned BaseReg, const MCExpr *Offset, SMLoc S, SMLoc E) {
    auto Op = make_unique<NaplesPUOperand>(Memory);
    Op->Mem.BaseReg = BaseReg;
    Op->Mem.Off = Offset;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }
};
} // end anonymous namespace.

// Auto-generated by TableGen
static unsigned MatchRegisterName(StringRef Name);

bool NaplesPUAsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                             OperandVector &Operands,
                                             MCStreamer &Out,
                                             uint64_t &ErrorInfo,
                                             bool MatchingInlineAsm) {
  MCInst Inst;
  SMLoc ErrorLoc;
  SmallVector<std::pair<unsigned, std::string>, 4> MapAndConstraints;

  switch (MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm)) {
  default:
    break;
  case Match_Success:
  /*
    Out.EmitInstruction(Inst, getSTI());
    return false;
    */
    if (ProcessInstruction(Inst, IDLoc, Out))
      return true;
    return false;
  case Match_MissingFeature:
    return Error(IDLoc, "Instruction use requires option to be enabled");
  case Match_MnemonicFail:
    return Error(IDLoc, "Unrecognized instruction mnemonic");
  case Match_InvalidOperand:
    ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(IDLoc, "Too few operands for instruction");

      ErrorLoc = ((NaplesPUOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }

    return Error(ErrorLoc, "Invalid operand for instruction");
  }

  llvm_unreachable("Unknown match type detected!");
}


bool NaplesPUAsmParser::ProcessInstruction(MCInst &Inst, const SMLoc &Loc,
                                        MCStreamer &Out) {
// TODO: (Catello) Add symbol to loadh and loadl.
  switch (Inst.getOpcode()) {
   case NaplesPU::LOAD_EFFECTIVE_ADDR_SYM: {
    const MCExpr *Symbol = Inst.getOperand(1).getExpr();

    // Load high bits
    MCInst NewInst1;
    NewInst1.setOpcode(NaplesPU::LEAH);
    NewInst1.addOperand(Inst.getOperand(0)); // Dest
    NewInst1.addOperand(MCOperand::createExpr(Symbol)); // Addr
    NewInst1.setLoc(Loc);
    Out.EmitInstruction(NewInst1, getSTI());

    // Load low bits
    MCInst NewInst2;
    NewInst2.setOpcode(NaplesPU::LEAL);
    NewInst2.addOperand(Inst.getOperand(0)); // Dest
    NewInst2.addOperand(MCOperand::createExpr(Symbol)); // Source
    NewInst2.setLoc(Loc);
    Out.EmitInstruction(NewInst2, getSTI());
    break;
  }

  default:
    Inst.setLoc(Loc);
    Out.EmitInstruction(Inst, getSTI());
  }

  return false;
}


bool NaplesPUAsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                   SMLoc &EndLoc) {
  StartLoc = Parser.getTok().getLoc();
  EndLoc = Parser.getTok().getEndLoc();

  switch (getLexer().getKind()) {
  default:
    return true;
  case AsmToken::Identifier:
    RegNo = MatchRegisterName(getLexer().getTok().getIdentifier());
    if (RegNo == 0)
      return true;

    getLexer().Lex();
    return false;
  }

  return true;
}

bool NaplesPUAsmParser::ParseImmediate(OperandVector &Ops) {
  SMLoc S = Parser.getTok().getLoc();
  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);

  const MCExpr *EVal;
  switch (getLexer().getKind()) {
  default:
    return true;
  case AsmToken::Plus:
  case AsmToken::Minus:
  case AsmToken::Integer:
    if (getParser().parseExpression(EVal))
      return true;

    int64_t ans;
    EVal->evaluateAsAbsolute(ans);
    Ops.push_back(NaplesPUOperand::createImm(EVal, S, E));
    return false;
  }
}

bool NaplesPUAsmParser::ParseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // Check if the current operand has a custom associated parser, if so, try to
  // custom parse the operand, or fallback to the general approach.
  OperandMatchResultTy ResTy = MatchOperandParserImpl(Operands, Mnemonic);
  if (ResTy == MatchOperand_Success)
    return false;

  unsigned RegNo;

  SMLoc StartLoc;
  SMLoc EndLoc;

  // Attempt to parse token as register
  if (!ParseRegister(RegNo, StartLoc, EndLoc)) {
    Operands.push_back(NaplesPUOperand::createReg(RegNo, StartLoc, EndLoc));
    return false;
  }

  if (!ParseImmediate(Operands))
    return false;

  // Identifier
  const MCExpr *IdVal;
  SMLoc S = Parser.getTok().getLoc();
  if (!getParser().parseExpression(IdVal)) {
    SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
    Operands.push_back(NaplesPUOperand::createImm(IdVal, S, E));
    return false;
  }

  // Error
  Error(Parser.getTok().getLoc(), "unknown operand");
  return true;
}

OperandMatchResultTy
NaplesPUAsmParser::ParseMemoryOperand(OperandVector &Operands) {
  SMLoc S = Parser.getTok().getLoc();
  if (getLexer().is(AsmToken::Identifier)) {
    // PC relative memory label memory access
    // load_32 s0, aLabel

    const MCExpr *IdVal;
    if (getParser().parseExpression(IdVal))
      return MatchOperand_ParseFail; // Bad identifier

    SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);

    // This will be turned into a PC relative load.
    Operands.push_back(
        NaplesPUOperand::createMem(MatchRegisterName("pc"), IdVal, S, E));
    return MatchOperand_Success;
  }

  const MCExpr *Offset;
  if (getLexer().is(AsmToken::Integer) || getLexer().is(AsmToken::Minus) ||
      getLexer().is(AsmToken::Plus)) {
    if (getParser().parseExpression(Offset))
      return MatchOperand_ParseFail;
  } else
    Offset = NULL;

  if (!getLexer().is(AsmToken::LParen)) {
    Error(Parser.getTok().getLoc(), "bad memory operand, missing (");
    return MatchOperand_ParseFail;
  }

  getLexer().Lex();
  unsigned RegNo;
  SMLoc _S, _E;
  if (ParseRegister(RegNo, _S, _E)) {
    Error(Parser.getTok().getLoc(), "bad memory operand: invalid register");
    return MatchOperand_ParseFail;
  }

  if (getLexer().isNot(AsmToken::RParen)) {
    Error(Parser.getTok().getLoc(), "bad memory operand, missing )");
    return MatchOperand_ParseFail;
  }

  getLexer().Lex();

  SMLoc E = SMLoc::getFromPointer(Parser.getTok().getLoc().getPointer() - 1);
  Operands.push_back(NaplesPUOperand::createMem(RegNo, Offset, S, E));

  return MatchOperand_Success;
}

bool NaplesPUAsmParser::ParseInstruction(ParseInstructionInfo &Info,
                                      StringRef Mnemonic, SMLoc NameLoc,
                                      OperandVector &Operands) {

  // First operand in MCInst is instruction mnemonic.
  Operands.push_back(NaplesPUOperand::createToken(Mnemonic, NameLoc));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // parse operands
  for (;;) {
    if (ParseOperand(Operands, Mnemonic))
      return true;

    if (getLexer().isNot(AsmToken::Comma))
      break;

    // Consume comma token
    getLexer().Lex();
  }

  return false;
}

bool NaplesPUAsmParser::ParseDirective(AsmToken DirectiveID) { return true; }

extern "C" void LLVMInitializeNaplesPUAsmParser() {
  RegisterMCAsmParser<NaplesPUAsmParser> X(TheNaplesPUTarget);
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "NaplesPUGenAsmMatcher.inc"
