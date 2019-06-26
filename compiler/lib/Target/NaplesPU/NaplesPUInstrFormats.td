//===-- NaplesPUInstrFormats.td - NaplesPU Instruction Formats ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction Pattern Stuff
//===----------------------------------------------------------------------===//

def simm16  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def simm9  : PatLeaf<(imm), [{ return isInt<9>(N->getSExtValue()); }]>;

// Addressing modes as in SPARC
def ADDRri : ComplexPattern<iPTR, 2, "SelectADDRri", [frameindex], []>;
def V16ADDRri : ComplexPattern<v16i32, 2, "SelectADDRri", [], []>;
def V8ADDRri : ComplexPattern<v8i64, 2, "SelectADDRri", [], []>;
//TODO: commentati perchè non dovrebbero servire
//def V16F32ADDRri : ComplexPattern<v16f32, 2, "SelectADDRri", [], []>;
//def V8F64ADDRri : ComplexPattern<v8f64, 2, "SelectADDRri", [], []>;
//def I64ADDRri : ComplexPattern<i64, 2, "SelectADDRri", [frameindex], []>;
//def F64ADDRri : ComplexPattern<f64, 2, "SelectADDRri", [frameindex], []>;

//===----------------------------------------------------------------------===//
// NaplesPU profiles and nodes
//===----------------------------------------------------------------------===//
// Transformation nodes
def LO32I : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getAPIntValue().getLoBits(32).getZExtValue(), SDLoc(N), MVT::i32);}]>;

def HI32I : SDNodeXForm<imm, [{
  // Transformation function: shift the immediate value down into the low bits.
  return CurDAG->getTargetConstant((unsigned)N->getAPIntValue().getHiBits(32).getZExtValue(), SDLoc(N),  MVT::i32);}]>;

def LO32F : SDNodeXForm<fpimm, [{
  return CurDAG->getTargetConstant((unsigned)(N->getValueAPF().bitcastToAPInt().getLoBits(32).getZExtValue()), SDLoc(N), MVT::i32);}]>;

def HI32F : SDNodeXForm<fpimm, [{
  // Transformation function: shift the immediate value down into the low bits.
  return CurDAG->getTargetConstant((unsigned)(N->getValueAPF().bitcastToAPInt().getHiBits(32).getZExtValue()), SDLoc(N),  MVT::i32);}]>;

/*
// Transformation nodes
def LO16I : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getZExtValue() & 0xffff, SDLoc(N), MVT::i32);}]>;

def HI16I : SDNodeXForm<imm, [{
  // Transformation function: shift the immediate value down into the low bits.
  return CurDAG->getTargetConstant(((unsigned)N->getZExtValue() >> 16) & 0xffff, SDLoc(N),  MVT::i32);}]>;
*/

def DIV2 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((unsigned)N->getZExtValue() / 2, SDLoc(N), MVT::i32);}]>;

// Moveil/moveih nodes definition, used for globaladdress lowering
def leah  : SDNode<"NaplesPUISD::LEAH", SDTypeProfile<1, 1, []>>;
def leal  : SDNode<"NaplesPUISD::LEAL", SDTypeProfile<1, 2, []>>;

// A splat is a vector with the same value in all lanes. Used to handle operation
// with both vector and scalar operands.
def splat : SDNode<"NaplesPUISD::SPLAT", SDTypeProfile<1, 1, [SDTCisEltOfVec<1, 0>]>>;

def return : SDNode<"NaplesPUISD::RET_FLAG", SDTypeProfile<0, 0, []>, 
	[SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_SPCall : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;

def call : SDNode<"NaplesPUISD::CALL", SDT_SPCall, 
	[SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic]>;

//To mark the beginning and end of a call sequence
def SDT_SPCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
 def SDT_SPCallSeqEnd : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
 def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_SPCallSeqStart,
                            [SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;
 def callseq_end : SDNode<"ISD::CALLSEQ_END", SDT_SPCallSeqEnd,
                            [SDNPHasChain, SDNPSideEffect,
                             SDNPOptInGlue, SDNPOutGlue]>; 
//To handle the lack of conditional moves
def selcondresult : SDNode<"NaplesPUISD::SEL_COND_RESULT", SDTypeProfile<1, 3, 
							[SDTCisSameAs<0, 2>, SDTCisSameAs<0, 3>]>>;

//===----------------------------------------------------------------------===//
// Operand Definitions
//===----------------------------------------------------------------------===//

// Used for the LEA_Sym, to detect the lea pseudo instruction
def symref : Operand<OtherVT> {}

def SIMM16OP : Operand<i32> {
  let DecoderMethod = "decodeSimm16Value";
}

def SIMM9OP : Operand<i32> {
  let DecoderMethod = "decodeSimm9Value";
}

def MemAsmOperand : AsmOperandClass {
  let Name = "Mem";
  let ParserMethod = "ParseMemoryOperand";
}

def MEMri : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeMemoryOpValue";
  let DecoderMethod = "decodeScalarMemoryOpValue";
  let ParserMatchClass = MemAsmOperand;
  let MIOperandInfo = (ops GPR32, i32imm);
}

def V16MEMri : Operand<v16i32> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeMemoryOpValue";
  let DecoderMethod = "decodeVectorWMemoryOpValue";
  let ParserMatchClass = MemAsmOperand;
  let MIOperandInfo = (ops VR512W, i32imm);
}
/*
def V8MEMri : Operand<v8i64> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeMemoryOpValue";
  let DecoderMethod = "decodeVectorLMemoryOpValue";
  let ParserMatchClass = MemAsmOperand;
  let MIOperandInfo = (ops VR512L, i32imm);
}
*/
def LEAri : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeLEAValue";
  let ParserMatchClass = MemAsmOperand; //TODO: controllare se è corretto il ParserMatchClass
  let MIOperandInfo = (ops GPR32, i32imm);
}

def ABSh : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeABShValue";
  let ParserMatchClass = MemAsmOperand; //TODO: controllare se è corretto il ParserMatchClass
  let MIOperandInfo = (ops i32imm);
}

def ABSl : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let EncoderMethod = "encodeABSlValue";
  let ParserMatchClass = MemAsmOperand; //TODO: controllare se è corretto il ParserMatchClass
  let MIOperandInfo = (ops i32imm);
}


def brtarget : Operand<OtherVT>
{
  let EncoderMethod = "encodeBranchTargetOpValue";
  let DecoderMethod = "decodeBranchTargetOpValue";
}

def calltarget : Operand<iPTR>
{
  let EncoderMethod = "encodeBranchTargetOpValue";
  let DecoderMethod = "decodeBranchTargetOpValue";
}

//===----------------------------------------------------------------------===//
// Pattern fragments
//===----------------------------------------------------------------------===//

// PatFrag used for trunc from i64 to i32
/*def GetEvenReg: OutPatFrag<(ops node:$Rs),
                      (EXTRACT_SUBREG (i64 $Rs), sub_even)>;
def GetOddReg: OutPatFrag<(ops node:$Rs),
                      (EXTRACT_SUBREG (i64 $Rs), sub_odd)>;
					  
def SetEvenReg: OutPatFrag<(ops node:$Rs),
                      (i64 (SUBREG_TO_REG (i64 0), (i32 $Rs), sub_even))>;
def SetOddReg: OutPatFrag<(ops node:$Rs),
                      (i64 (SUBREG_TO_REG (i64 0), (i32 $Rs), sub_odd))>;
*/// PatFrag for scratchpad

// Definition of anyextload used in the loading of vector types < 512 bits
def anyextload : PatFrag<(ops node:$ptr), (unindexedload node:$ptr), 
    [{ return cast<LoadSDNode>(N)->getExtensionType() != ISD::NON_EXTLOAD;}]>; 

//----------------------------------------------------------------------------// 
//------------------------------ LOAD AND STORE ------------------------------// 
//----------------------------------------------------------------------------// 

def MemStore : PatFrag<(ops node:$val, node:$ptr), (store node:$val, node:$ptr), [{
                            if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
                              return !cast<StoreSDNode>(N)->isTruncatingStore();
                            else
                              return false;}]>;

def MemLoad : PatFrag<(ops node:$ptr), (load node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getExtensionType() == ISD::NON_EXTLOAD;
          else
            return false;}]>;

def ScratchpadStore : PatFrag<(ops node:$val, node:$ptr), (store node:$val, node:$ptr), [{
                            if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
                              return !cast<StoreSDNode>(N)->isTruncatingStore();
                            else
                              return false;}]>;

def ScratchpadLoad : PatFrag<(ops node:$ptr), (load node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getExtensionType() == ISD::NON_EXTLOAD;
          else
            return false;}]>;

//---------------- EXTLOAD scalar ----------------// 
def extloadi1_mem : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;
def extloadi1_scratch : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;

def extloadi8_mem : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;
def extloadi8_scratch : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;

def extloadi16_mem : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;
def extloadi16_scratch : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;

def extloadi32_mem : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;
def extloadi32_scratch : PatFrag<(ops node:$ptr), (extload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;

//---------------- ZEXTLOAD scalar ----------------// 
def zextloadi1_mem : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;
def zextloadi1_scratch : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;

def zextloadi8_mem : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;
def zextloadi8_scratch : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;

def zextloadi16_mem : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;
def zextloadi16_scratch : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;

def zextloadi32_mem : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;
def zextloadi32_scratch : PatFrag<(ops node:$ptr), (zextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;

//---------------- ZEXTLOAD vector ----------------// 
def zextloadv16i8_mem: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
      else
        return false; }]>;
def zextloadv16i8_scratch: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
       else
         return false; }]>;

def zextloadv16i16_mem: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
      else
        return false; }]>;
def zextloadv16i16_scratch: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
       else
         return false; }]>;

def zextloadv8i8_mem: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
      else
        return false; }]>;
def zextloadv8i8_scratch: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
       else
         return false; }]>;

def zextloadv8i16_mem: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
      else
        return false; }]>;
def zextloadv8i16_scratch: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
       else
         return false; }]>;

def zextloadv8i32_mem: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
      else
        return false; }]>;
def zextloadv8i32_scratch: PatFrag<(ops node:$ptr), (zextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
       else
         return false; }]>;                  


//---------------- SEXTLOAD scalar ----------------// 
def sextloadi1_mem : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;
def sextloadi1_scratch : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;

def sextloadi8_mem : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;
def sextloadi8_scratch : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;

def sextloadi16_mem : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;
def sextloadi16_scratch : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;

def sextloadi32_mem : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;
def sextloadi32_scratch : PatFrag<(ops node:$ptr), (sextload node:$ptr), [{
          if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
            return cast<LoadSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;
			
//---------------- SEXTLOAD vector ----------------// 
def sextloadv16i8_mem: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
      else
        return false; }]>;
def sextloadv16i8_scratch: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
       else
         return false; }]>;

def sextloadv16i16_mem: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
      else
        return false; }]>;
def sextloadv16i16_scratch: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
       else
         return false; }]>;

def sextloadv8i8_mem: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
      else
        return false; }]>;
def sextloadv8i8_scratch: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
       else
         return false; }]>;

def sextloadv8i16_mem: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
      else
        return false; }]>;
def sextloadv8i16_scratch: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
       else
         return false; }]>;

def sextloadv8i32_mem: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
      else
        return false; }]>;
def sextloadv8i32_scratch: PatFrag<(ops node:$ptr), (sextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
       else
         return false; }]>;                                    

		 			
//---------------- ANYEXTLOAD vector ----------------// 
def anyextloadv16i8_mem: PatFrag<(ops node:$ptr), (anyextload node:$ptr), 
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
       else
         return false; }]>;
def anyextloadv16i8_scratch: PatFrag<(ops node:$ptr), (anyextload node:$ptr), 
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i8;
       else
         return false; }]>;

def anyextloadv16i16_mem: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
      else
        return false; }]>;
def anyextloadv16i16_scratch: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v16i16;
       else
         return false; }]>;

def anyextloadv8i8_mem: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
      else
        return false; }]>;
def anyextloadv8i8_scratch: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i8;
       else
         return false; }]>;         

def anyextloadv8i16_mem: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
      else
        return false; }]>;
def anyextloadv8i16_scratch: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i16;
       else
         return false; }]>;

def anyextloadv8i32_mem: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() != 68)
        return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
      else
        return false; }]>;
def anyextloadv8i32_scratch: PatFrag<(ops node:$ptr), (anyextload node:$ptr),
    [{ if(cast<LoadSDNode>(N)->getAddressSpace() == 68)
         return cast<LoadSDNode>(N)->getMemoryVT() == MVT::v8i32;
       else
         return false; }]>;

		 
//---------------- TRUNCSTORE scalar ----------------// 
def truncstorei1_mem : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;
def truncstorei1_scratch : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i1;
          else
            return false;}]>;

def truncstorei8_mem : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;
def truncstorei8_scratch : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i8;
          else
            return false;}]>;

def truncstorei16_mem : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;
def truncstorei16_scratch : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i16;
          else
            return false;}]>;

def truncstorei32_mem : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;
def truncstorei32_scratch : PatFrag<(ops node:$val, node:$ptr),
                           (truncstore node:$val, node:$ptr), [{
          if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::i32;
          else
            return false;}]>;

//---------------- TRUNCSTORE vector ----------------// 
def truncstorev16i8_mem: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v16i8;
          else
            return false; }]>;
def truncstorev16i8_scratch: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v16i8;
          else
            return false; }]>;   

def truncstorev16i16_mem: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v16i16;
          else
            return false; }]>;
def truncstorev16i16_scratch: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v16i16;
          else
            return false; }]>;

def truncstorev8i8_mem: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i8;
          else
            return false; }]>;
def truncstorev8i8_scratch: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i8;
          else
            return false; }]>;

def truncstorev8i16_mem: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i16;
          else
            return false; }]>;
def truncstorev8i16_scratch: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i16;
          else
            return false; }]>;

def truncstorev8i32_mem: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() != 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i32;
          else
            return false; }]>;
def truncstorev8i32_scratch: PatFrag<(ops node:$val, node:$ptr), (truncstore node:$val, node:$ptr),
    [{ if(cast<StoreSDNode>(N)->getAddressSpace() == 68)
            return cast<StoreSDNode>(N)->getMemoryVT() == MVT::v8i32;
          else
            return false; }]>;                                                         

			
// insertelt SDNode redefinition
def VecInsert : SDTypeProfile<1, 3, [    // vector insert
   SDTCisSameAs<0, 1>, SDTCisPtrTy<3>
]>;

def insert_elt  : SDNode<"ISD::INSERT_VECTOR_ELT", VecInsert>;

//===----------------------------------------------------------------------===//
//  Describe NaplesPU Special Registers
//
//
//===----------------------------------------------------------------------===//

class SpReg<bits<6> reg> {
  bits<6> Register = reg;
}

def MaskReg : SpReg<59>;

//===----------------------------------------------------------------------===//
//  Describe NaplesPU scalar or vector instructions
//
//  Fmt   - 0 if a register is scalar, 1 if vector
//===----------------------------------------------------------------------===//

class Fmt<bit val> {
  bit Value = val;
}

def Fmt_S : Fmt<0>;
def Fmt_V : Fmt<1>;

//===----------------------------------------------------------------------===//
// Describe NaplesPU instructions format here
//===----------------------------------------------------------------------===//

class InstNaplesPU<dag outs, dag ins, string asmstr, list<dag> pattern>
          : Instruction {
  field bits<32> Inst;

  let Namespace = "NaplesPU";
  let Size = 4;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;

  //let DecoderNamespace = "NaplesPU";
  field bits<32> SoftFail = 0;
}


//===----------------------------------------------------------------------===//
// Format R instruction class in NaplesPU : <00|opcode|rd|rs0|rs1|unused|l|fmt|m|>
// l: if 1, 64 bit mode
// fmt2: FMT value for rd register
// fmt1: FMT value for rs0 register
// fmt0: FMT value for rs1 register
// m: if 1, masked
//
//===----------------------------------------------------------------------===//

class FR<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1, Fmt fmt0, bit m>
   : InstNaplesPU<outs, ins, asmstr, pattern> {
  bits <6> dst;
  bits <6> src0;

  let Inst{31-30} = 0;
  let Inst{29-24} = opcode;
  let Inst{23-18} = dst;
  let Inst{17-12} = src0;
  let Inst{5} = 0; //unused
  let Inst{4} = l;
  let Inst{3} = fmt2.Value;
  let Inst{2} = fmt1.Value;
  let Inst{1} = fmt0.Value;
  let Inst{0} = m;
}

class FR_TwoOp<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1, Fmt fmt0, bit m>
   : FR<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, fmt0, m> {
  bits <6> src1;

  let Inst{11-6} = src1;
}

class FR_OneOp<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1, Fmt fmt0, bit m>
   : FR<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, fmt0, m> {

  let Inst{11-6} = 0;
}

class FR_TwoOp_Masked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, fmt0, 1> {
  let Uses = [MR_REG];
}

class FR_TwoOp_Unmasked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, fmt0, 0> {

}

class FR_OneOp_Masked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1>
   : FR_OneOp<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, Fmt_S, 1> {
  let Uses = [MR_REG];
}

class FR_OneOp_Unmasked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, Fmt fmt2, Fmt fmt1>
   : FR_OneOp<outs, ins, asmstr, pattern, opcode, l, fmt2, fmt1, Fmt_S, 0> {

}

class FR_TwoOp_Masked_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp_Masked<outs, ins, asmstr, pattern, opcode, 0, fmt2, fmt1, fmt0> {

}

class FR_TwoOp_Masked_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp_Masked<outs, ins, asmstr, pattern, opcode, 1, fmt2, fmt1, fmt0> {

}

class FR_TwoOp_Unmasked_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp_Unmasked<outs, ins, asmstr, pattern, opcode, 0, fmt2, fmt1, fmt0> {

}

class FR_TwoOp_Unmasked_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1, Fmt fmt0>
   : FR_TwoOp_Unmasked<outs, ins, asmstr, pattern, opcode, 1, fmt2, fmt1, fmt0> {

}

class FR_OneOp_Masked_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1>
   : FR_OneOp_Masked<outs, ins, asmstr, pattern, opcode, 0, fmt2, fmt1> {

}

class FR_OneOp_Masked_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1>
   : FR_OneOp_Masked<outs, ins, asmstr, pattern, opcode, 1, fmt2, fmt1> {

}

class FR_OneOp_Unmasked_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1>
   : FR_OneOp_Unmasked<outs, ins, asmstr, pattern, opcode, 0, fmt2, fmt1> {

}

class FR_OneOp_Unmasked_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, Fmt fmt2, Fmt fmt1>
   : FR_OneOp_Unmasked<outs, ins, asmstr, pattern, opcode, 1, fmt2, fmt1> {

}

//===----------------------------------------------------------------------===//
// Format I instruction class in NaplesPU : <010|opcode|rd|rs|imm|fmt|m|>
// fmt1: FMT value for rd register
// fmt0: FMT value for rs register
// m: if 1 masked
//===----------------------------------------------------------------------===//

class FI<dag outs, dag ins, string asmstr, list<dag> pattern, bits<5> opcode, Fmt fmt1, Fmt fmt0, bit m>
    : InstNaplesPU<outs, ins, asmstr, pattern> {
  bits <6> dst;
  bits <9> imm;

  let Inst{31-29} = 0b010;
  let Inst{28-24} = opcode;
  let Inst{23-18} = dst;
  let Inst{11-3} = imm;
  let Inst{2} = fmt1.Value;
  let Inst{1} = fmt0.Value;
  let Inst{0} = m;
}

class FI_OneOp<dag outs, dag ins, string asmstr, list<dag> pattern, bits<5> opcode, Fmt fmt1, Fmt fmt0, bit m> : FI<outs, ins, asmstr, pattern, opcode, fmt1, fmt0, m> {
  bits <6> src;

  let Inst{17-12} = src;
}

class FI_NoOp<dag outs, dag ins, string asmstr, list<dag> pattern, bits<5> opcode, Fmt fmt1> : FI<outs, ins, asmstr, pattern, opcode, fmt1, Fmt_S, 0> {
  let Inst{17-12} = 0;
}

class FI_OneOp_Masked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<5> opcode, Fmt fmt1, Fmt fmt0> : FI_OneOp<outs, ins, asmstr, pattern, opcode, fmt1, fmt0, 1> {
  let Uses = [MR_REG];
}

class FI_OneOp_Unmasked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<5> opcode, Fmt fmt1, Fmt fmt0> : FI_OneOp<outs, ins, asmstr, pattern, opcode, fmt1, fmt0, 0> {

}

//===----------------------------------------------------------------------===//
// Format MOVEI instruction class in NaplesPU : <01100|opcode|rd|imm|fmt|m|>
//===----------------------------------------------------------------------===//

class FMOVEI<dag outs, dag ins, string asmstr, list<dag> pattern, bits<3> opcode, Fmt fmt, bit m>
      : InstNaplesPU<outs, ins, asmstr, pattern> {
  bits <6> dst;
  bits <16> imm;

  let Inst{31-27} = 0b01100;
  let Inst{26-24} = opcode;
  let Inst{23-18} = dst;
  let Inst{17-2} = imm;
  let Inst{1} = fmt.Value;
  let Inst{0} = m;
}

//===----------------------------------------------------------------------===//
// Format M instruction class in NaplesPU : <10|opcode|rd/rs|rptr|off|l|s|m|>
//===----------------------------------------------------------------------===//

class FM<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, bit s, bit m>
    : InstNaplesPU<outs, ins, asmstr, pattern> {
  bits <6> dstsrc;
  bits <15> addr; //base address and offset encoded on the same 15 bits value (check encodeMemoryOpValue)

  let Inst{31-30} = 0b10;
  let Inst{29-24} = opcode;
  let Inst{23-18} = dstsrc;
  let Inst{17-12} = addr{5-0};
  let Inst{11-3} = addr{14-6};
  let Inst{2} = l;
  let Inst{1} = s;
  let Inst{0} = m;
}

class FM_Unmasked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, bit s>
   : FM<outs, ins, asmstr, pattern, opcode, l, s, 0> {
}

class FM_Masked<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l, bit s>
   : FM<outs, ins, asmstr, pattern, opcode, l, s, 1> {
  let Uses = [MR_REG];
}

class FM_Unmasked_Mainmem<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l>
   : FM_Unmasked<outs, ins, asmstr, pattern, opcode, l, 0> {
}

class FM_Unmasked_Scratchpad<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l>
   : FM_Unmasked<outs, ins, asmstr, pattern, opcode, l, 1> {
}

class FM_Masked_Mainmem<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l>
   : FM_Masked<outs, ins, asmstr, pattern, opcode, l, 0> {
}

class FM_Masked_Scratchpad<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode, bit l>
   : FM_Masked<outs, ins, asmstr, pattern, opcode, l, 1> {
}

class FM_Unmasked_Mainmem_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Unmasked_Mainmem<outs, ins, asmstr, pattern, opcode, 0> {
}

class FM_Unmasked_Scratchpad_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Unmasked_Scratchpad<outs, ins, asmstr, pattern, opcode, 0> {
}

class FM_Masked_Mainmem_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Masked_Mainmem<outs, ins, asmstr, pattern, opcode, 0> {
}

class FM_Masked_Scratchpad_32<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Masked_Scratchpad<outs, ins, asmstr, pattern, opcode, 0> {
}

class FM_Unmasked_Mainmem_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Unmasked_Mainmem<outs, ins, asmstr, pattern, opcode, 1> {
}

class FM_Unmasked_Scratchpad_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Unmasked_Scratchpad<outs, ins, asmstr, pattern, opcode, 1> {
}

class FM_Masked_Mainmem_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Masked_Mainmem<outs, ins, asmstr, pattern, opcode, 1> {
}

class FM_Masked_Scratchpad_64<dag outs, dag ins, string asmstr, list<dag> pattern, bits<6> opcode>
   : FM_Masked_Scratchpad<outs, ins, asmstr, pattern, opcode, 1> {
}


//===----------------------------------------------------------------------===//
// Format J/BR instruction class in NaplesPU 
// FJR: <0111|type(0/1)|opcode|rd|imm|>
// FJ:  <0111|type(0/1)|opcode|imm|>
//===----------------------------------------------------------------------===//

class FJ_ALL<dag outs, dag ins, string asmstr, list<dag> pattern, bits<3> opcode>
    : InstNaplesPU<outs, ins, asmstr, pattern> {

  let Inst{31-28} = 0b0111;
  let Inst{26-24} = opcode;
  let isBranch = 1;
}

class FJR<dag outs, dag ins, string asmstr, list<dag> pattern, bits<3> opcode>
    : FJ_ALL<outs, ins, asmstr, pattern, opcode> {
  bits <6> cond;
  bits <18> addr;

  let Inst{27} = 0;
  let Inst{23-18} = cond;
  let Inst{17-0} = addr;
}

class FJ<dag outs, dag ins, string asmstr, list<dag> pattern, bits<3> opcode>
    : FJ_ALL<outs, ins, asmstr, pattern, opcode> {
  bits <24> addr;

  let Inst{27} = 1;
  let Inst{23-0} = addr;
}

//===----------------------------------------------------------------------===//
// Format C instruction class in NaplesPU 
// FC:  <01101|opcode|rs0|rs1|unused|>
//===----------------------------------------------------------------------===//

class FC<dag outs, dag ins, string asmstr, list<dag> pattern, bits<3> opcode>
    : InstNaplesPU<outs, ins, asmstr, pattern> {

  bits <6> src0;
  bits <6> src1;

  let Inst{31-27} = 0b01101;
  let Inst{26-24} = opcode;
  let Inst{23-18} = src0;
  let Inst{17-12} = src1;
  let Inst{11-0} = 0; //unused
}

//===----------------------------------------------------------------------===//
//  A set of multiclasses is used to handle Vector/Scalar combinations
//   SS: Scalar = Op Scalar
//   VV: Vector = Op Vector
//   SI: Vector = Op Immediate
//  SSS: Scalar = Scalar Op Scalar
//  VVS: Vector = Vector Op Scalar
//  VVV: Vector = Vector Op Vector
//  SVV: Scalar = Vector Op Vector
//  SSI: Scalar = Vector Op Immediate
//  VVI: Scalar = Vector Op Immediate
//===----------------------------------------------------------------------===//

multiclass FArithInt_TwoOp<string operator, SDNode OpNode, bits<6> opcode> {
  // FR - SSS - 32 bit integer
  def SSS_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set i32:$dst, (OpNode i32:$src0, i32:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;

/*  // FR - SSS - 64 bit integer
  def SSS_64 : FR_TwoOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0, GPR64:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set i64:$dst, (OpNode i64:$src0, i64:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
*/
  // FR - VVS unmasked - 32 bit integer
  def VVS_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set v16i32:$dst, (OpNode v16i32:$src0, (v16i32 (splat i32:$src1))))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;
/*
  // FR - VVS unmasked - 64 bit integer
  def VVS_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, GPR64:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set v8i64:$dst, (OpNode v8i64:$src0, (splat i64:$src1)))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;
*/
  // FR - VVV unmasked - 32 bit integer
  def VVV_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set v16i32:$dst, (OpNode v16i32:$src0, v16i32:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;
/*  
  // FR - VVV unmasked - 64 bit integer
  def VVV_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set v8i64:$dst, (OpNode v8i64:$src0, v8i64:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;
*/
  let Constraints = "$dst = $oldvalue" in {
    // FR - VVS masked - 32 bit integer
    def VVS_M_32 : FR_TwoOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, GPR32:$src1, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0, $src1",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0, (v16i32 (splat i32:$src1))),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_S>;

 /*   // FR - VVS masked - 64 bit integer
    def VVS_M_64 : FR_TwoOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, GPR64:$src1, VR512L:$oldvalue),
      operator # "_i64.m $dst, $src0, $src1",
      [(set v8i64:$dst, (int_npu_vector_mixi64 (OpNode v8i64:$src0, (splat i64:$src1)),
        v8i64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_S>;
*/
    // FR - VVV masked - 32 bit integer
    def VVV_M_32 : FR_TwoOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, VR512W:$src1, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0, $src1",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0, v16i32:$src1),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_V>;
/*
    // FR - VVV masked - 64 bit integer
  def VVV_M_64 : FR_TwoOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$src1, VR512L:$oldvalue),
    operator # "_i64.m $dst, $src0, $src1",
    [(set v8i64:$dst, (int_npu_vector_mixi64 (OpNode v8i64:$src0, v8i64:$src1),
        v8i64:$oldvalue))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>; */
  }
  // FI - SSI
  def SSI : FI_OneOp_Unmasked<
    (outs GPR32:$dst),
    (ins GPR32:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set i32:$dst, (OpNode i32:$src, (i32 simm9:$imm)))],
    opcode{4-0},
    Fmt_S,
    Fmt_S>;

  // FI - VVI unmasked
  def VVI_U : FI_OneOp_Unmasked<
    (outs VR512W:$dst),
    (ins VR512W:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set v16i32:$dst, (OpNode v16i32:$src, (v16i32 (splat simm9:$imm))))],
    opcode{4-0},
    Fmt_V,
    Fmt_V>;

  // FI - VVI masked
  let Constraints = "$dst = $oldvalue" in {
    def VVI_M : FI_OneOp_Masked<
    (outs VR512W:$dst),
    (ins VR512W:$src, SIMM9OP:$imm, VR512W:$oldvalue),
    operator # "i.m $dst, $src, $imm",
    [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src, (v16i32 (splat simm9:$imm))), v16i32:$oldvalue))],
    opcode{4-0},
    Fmt_V,
    Fmt_V>;
  }
}

multiclass FArithInt_OneOp<string operator, SDNode OpNode, bits<6> opcode> {
  // FR - SS - 32 bit integer
  def SS_32 : FR_OneOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0),
    operator # "_i32 $dst, $src0",
    [(set i32:$dst, (OpNode i32:$src0))],
    opcode,
    Fmt_S,
    Fmt_S>;
/*
  // FR - SS - 64 bit integer
  def SS_64 : FR_OneOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0),
    operator # "_i64 $dst, $src0",
    [(set i64:$dst, (OpNode i64:$src0))],
    opcode,
    Fmt_S,
    Fmt_S>;
*/
  // FR - VV unmasked - 32 bit integer
  def VV_U_32 : FR_OneOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0),
    operator # "_i32 $dst, $src0",
    [(set v16i32:$dst, (OpNode v16i32:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;

 /* // FR - VV unmasked - 64 bit integer
  def VV_U_64 : FR_OneOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0),
    operator # "_i64 $dst, $src0",
    [(set v8i64:$dst, (OpNode v8i64:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;
*/
  let Constraints = "$dst = $oldvalue" in {
    // FR - VV masked - 32 bit integer
    def VV_M_32 : FR_OneOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;

/*    // FR - VV masked - 64 bit integer
    def VV_M_64 : FR_OneOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, VR512L:$oldvalue),
      operator # "_i64.m $dst, $src0",
      [(set v8i64:$dst, (int_npu_vector_mixi64 (OpNode v8i64:$src0),
        v8i64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;*/
  }
}

// per la shift rotate
multiclass FSRInt_TwoOp<string operator, SDNode OpNode, bits<6> opcode> {
  // FR - SSS - 32 bit integer
  def SSS_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set i32:$dst, (OpNode i32:$src0, i32:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;

/*  // FR - SSS - 64 bit integer
  def SSS_64 : FR_TwoOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0, GPR32:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set i64:$dst, (OpNode i64:$src0, i32:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
*/
  // FR - VVS unmasked - 32 bit integer
  def VVS_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set v16i32:$dst, (OpNode v16i32:$src0, (v16i32 (splat i32:$src1))))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;

/*  // FR - VVS unmasked - 64 bit integer
  def VVS_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, GPR64:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set v8i64:$dst, (OpNode v8i64:$src0, (splat i64:$src1)))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;
*/
  // FR - VVV unmasked - 32 bit integer
  def VVV_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set v16i32:$dst, (OpNode v16i32:$src0, v16i32:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;

 /* // FR - VVV unmasked - 64 bit integer
  def VVV_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set v8i64:$dst, (OpNode v8i64:$src0, v8i64:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;
*/
  let Constraints = "$dst = $oldvalue" in {
    // FR - VVS masked - 32 bit integer
    def VVS_M_32 : FR_TwoOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, GPR32:$src1, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0, $src1",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0, (v16i32 (splat i32:$src1))),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_S>;

 /*   // FR - VVS masked - 64 bit integer
    def VVS_M_64 : FR_TwoOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, GPR64:$src1, VR512L:$oldvalue),
      operator # "_i64.m $dst, $src0, $src1",
      [(set v8i64:$dst, (int_npu_vector_mixi64 (OpNode v8i64:$src0, (splat i64:$src1)),
        v8i64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_S>;
*/
    // FR - VVV masked - 32 bit integer
    def VVV_M_32 : FR_TwoOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, VR512W:$src1, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0, $src1",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0, v16i32:$src1),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_V>;

 /*   // FR - VVV masked - 64 bit integer
    def VVV_M_64 : FR_TwoOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, VR512L:$src1, VR512L:$oldvalue),
      operator # "_i64.m $dst, $src0, $src1",
      [(set v8i64:$dst, (int_npu_vector_mixi64 (OpNode v8i64:$src0, v8i64:$src1),
        v8i64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V,
      Fmt_V>;*/
  }
  // FI - SSI
  def SSI : FI_OneOp_Unmasked<
    (outs GPR32:$dst),
    (ins GPR32:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set i32:$dst, (OpNode i32:$src, (i32 simm9:$imm)))],
    opcode{4-0},
    Fmt_S,
    Fmt_S>;

  // FI - VVI unmasked
  def VVI_U : FI_OneOp_Unmasked<
    (outs VR512W:$dst),
    (ins VR512W:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set v16i32:$dst, (OpNode v16i32:$src, (v16i32 (splat simm9:$imm))))],
    opcode{4-0},
    Fmt_V,
    Fmt_V>;

  // FI - VVI masked
  let Constraints = "$dst = $oldvalue" in {
    def VVI_M : FI_OneOp_Masked<
    (outs VR512W:$dst),
    (ins VR512W:$src, SIMM9OP:$imm, VR512W:$oldvalue),
    operator # "i.m $dst, $src, $imm",
    [(set v16i32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src, (v16i32 (splat simm9:$imm))), v16i32:$oldvalue))],
    opcode{4-0},
    Fmt_V,
    Fmt_V>;
  }
}

multiclass FArithFloat_TwoOp<string operator, SDNode OpNode, bits<6> opcode> {
  // FR - SSS - 32 bit float
  def SSS_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0, GPR32:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set f32:$dst, (OpNode f32:$src0, f32:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;

/*  // FR - SSS - 64 bit float
  def SSS_64 : FR_TwoOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0, GPR64:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set f64:$dst, (OpNode f64:$src0, f64:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
*/
  // FR - VVS unmasked - 32 bit float
  def VVS_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, GPR32:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set v16f32:$dst, (OpNode v16f32:$src0, (splat f32:$src1)))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;

 /* // FR - VVS unmasked - 64 bit float
  def VVS_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, GPR64:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set v8f64:$dst, (OpNode v8f64:$src0, (splat f64:$src1)))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;
*/
  // FR - VVV unmasked - 32 bit float
  def VVV_U_32 : FR_TwoOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set v16f32:$dst, (OpNode v16f32:$src0, v16f32:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;

 /* // FR - VVV unmasked - 64 bit float
  def VVV_U_64 : FR_TwoOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set v8f64:$dst, (OpNode v8f64:$src0, v8f64:$src1))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;
*/
  let Constraints = "$dst = $oldvalue" in {
    // FR - VVS masked - 32 bit float
    def VVS_M_32 : FR_TwoOp_Masked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, GPR32:$src1, VR512W:$oldvalue),
    operator # "_f32.m $dst, $src0, $src1",
    [(set v16f32:$dst, (int_npu_vector_mixf32 (OpNode v16f32:$src0, (splat f32:$src1)),
        v16f32:$oldvalue))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;

  /*  // FR - VVS masked - 64 bit float
    def VVS_M_64 : FR_TwoOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, GPR64:$src1, VR512L:$oldvalue),
    operator # "_f64.m $dst, $src0, $src1",
    [(set v8f64:$dst, (int_npu_vector_mixf64 (OpNode v8f64:$src0, (splat f64:$src1)),
        v8f64:$oldvalue))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_S>;
*/
    // FR - VVV masked - 32 bit float
    def VVV_M_32 : FR_TwoOp_Masked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0, VR512W:$src1, VR512W:$oldvalue),
    operator # "_f32.m $dst, $src0, $src1",
    [(set v16f32:$dst, (int_npu_vector_mixf32 (OpNode v16f32:$src0, v16f32:$src1),
        v16f32:$oldvalue))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;

    // FR - VVV masked - 64 bit float
/*    def VVV_M_64 : FR_TwoOp_Masked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0, VR512L:$src1, VR512L:$oldvalue),
    operator # "_f64.m $dst, $src0, $src1",
    [(set v8f64:$dst, (int_npu_vector_mixf64 (OpNode v8f64:$src0, v8f64:$src1),
        v8f64:$oldvalue))],
    opcode,
    Fmt_V,
    Fmt_V,
    Fmt_V>;
*/  }
}

multiclass FArithFloat_OneOp<string operator, SDNode OpNode, bits<6> opcode> {
  // FR - SS - 32 bit float
  def SS_32 : FR_OneOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0),
    operator # "_f32 $dst, $src0",
    [(set f32:$dst, (OpNode f32:$src0))],
    opcode,
    Fmt_S,
    Fmt_S>;

 /* // FR - SS - 64 bit float
  def SS_64 : FR_OneOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0),
    operator # "_f64 $dst, $src0",
    [(set f64:$dst, (OpNode f64:$src0))],
    opcode,
    Fmt_S,
    Fmt_S>;
*/
  // FR - VV unmasked - 32 bit float
  def VV_U_32 : FR_OneOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0),
    operator # "_f32 $dst, $src0",
    [(set v16f32:$dst, (OpNode v16f32:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;
/*
  // FR - VV unmasked - 64 bit float
  def VV_U_64 : FR_OneOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0),
    operator # "_f64 $dst, $src0",
    [(set v8f64:$dst, (OpNode v8f64:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;
*/
  let Constraints = "$dst = $oldvalue" in {
    // FR - VV masked - 32 bit float
    def VV_M_32 : FR_OneOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, VR512W:$oldvalue),
      operator # "_f32.m $dst, $src0",
      [(set v16f32:$dst, (int_npu_vector_mixi32 (OpNode v16i32:$src0),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;

  /*  // FR - VV masked - 64 bit float
    def VV_M_64 : FR_OneOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, VR512L:$oldvalue),
      operator # "_f64.m $dst, $src0",
      [(set v8f64:$dst, (int_npu_vector_mixi64 (OpNode v8f64:$src0),
        v8f64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;
 */ }
}

// Condition codes defined in include/llvm/CodeGen/ISDOpcodes.h
// VS and VV comparisons are handled through intrinsics
multiclass FCompInt<string operator, CondCode condition,
  bits<6> opcode, Intrinsic vectorIntr32> {
  // FR - SSS - 32 bit integer
  def SSS_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set i32:$dst, (setcc i32:$src0, i32:$src1, condition))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
/*
  // FR - SSS - 64 bit integer
  def SSS_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins GPR64:$src0, GPR64:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set i32:$dst, (setcc i64:$src0, i64:$src1, condition))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
  */
  // FR - SVS unmasked - 32 bit integer
  def SVS_U_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins VR512W:$src0, GPR32:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr32 v16i32:$src0, (splat i32:$src1)))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_S>;
/*
  // FR - SVS unmasked - 64 bit integer
  def SVS_U_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins VR512L:$src0, GPR64:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr64 v8i64:$src0, (splat i64:$src1)))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_S>;
*/
  // FR - SVV unmasked - 32 bit integer
  def SVV_U_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins VR512W:$src0, VR512W:$src1),
    operator # "_i32 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr32 v16i32:$src0, v16i32:$src1))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_V>;
/*
  // FR - SVV unmasked - 64 bit integer
  def SVV_U_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins VR512L:$src0, VR512L:$src1),
    operator # "_i64 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr64 v8i64:$src0, v8i64:$src1))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_V>;
*/
  // FI - SSI
  def SSI : FI_OneOp_Unmasked<
    (outs GPR32:$dst),
    (ins GPR32:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set i32:$dst, (setcc i32:$src, simm9:$imm, condition))],
    opcode{4-0},
    Fmt_S,
    Fmt_S>;

  // FI - SVI unmasked
  def SVI : FI_OneOp_Unmasked<
    (outs GPR32:$dst),
    (ins VR512W:$src, SIMM9OP:$imm),
    operator # "i $dst, $src, $imm",
    [(set i32:$dst, (vectorIntr32 v16i32:$src, (splat simm9:$imm)))],
    opcode{4-0},
    Fmt_S,
    Fmt_V>;
}

multiclass FCompFloat<string operator, SDNode OpNode,
  bits<6> opcode, Intrinsic vectorIntr32> {
  // FR - SSS - 32 bit float
  def SSS_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0, GPR32:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set i32:$dst, (OpNode f32:$src0, f32:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
/*
  // FR - SSS - 64 bit float
  def SSS_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins GPR64:$src0, GPR64:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set i32:$dst, (OpNode f64:$src0, f64:$src1))],
    opcode,
    Fmt_S,
    Fmt_S,
    Fmt_S>;
 */ 
  // FR - SVS unmasked - 32 bit float
  def SVS_U_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins VR512W:$src0, GPR32:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr32 v16f32:$src0, (splat f32:$src1)))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_S>;
/*
  // FR - SVS unmasked - 64 bit float
  def SVS_U_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins VR512L:$src0, GPR64:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr64 v8f64:$src0, (splat f64:$src1)))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_S>;
*/
  // FR - SVV unmasked - 32 bit float
  def SVV_U_32 : FR_TwoOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins VR512W:$src0, VR512W:$src1),
    operator # "_f32 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr32 v16f32:$src0, v16f32:$src1))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_V>;
/*
  // FR - SVV unmasked - 64 bit float
  def SVV_U_64 : FR_TwoOp_Unmasked_64<
    (outs GPR32:$dst),
    (ins VR512L:$src0, VR512L:$src1),
    operator # "_f64 $dst, $src0, $src1",
    [(set i32:$dst, (vectorIntr64 v8f64:$src0, v8f64:$src1))],
    opcode,
    Fmt_S,
    Fmt_V,
    Fmt_V>;*/
}

multiclass FSext_32<string operator, ValueType vt,
  bits<6> opcode, ValueType vt_v> {
  // FR - SS
  def SS : FR_OneOp_Unmasked_32<
    (outs GPR32:$dst),
    (ins GPR32:$src0),
    operator # "_i32 $dst, $src0",
    [(set i32:$dst, (sext_inreg i32:$src0, vt))],
    opcode,
    Fmt_S,
    Fmt_S>;

  // FR - VV unmasked
  def VV_U : FR_OneOp_Unmasked_32<
    (outs VR512W:$dst),
    (ins VR512W:$src0),
    operator # "_i32 $dst, $src0",
    [(set v16i32:$dst, (sext vt_v:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;

  let Constraints = "$dst = $oldvalue" in {
    // FR - VV masked
    def VV_M : FR_OneOp_Masked_32<
      (outs VR512W:$dst),
      (ins VR512W:$src0, VR512W:$oldvalue),
      operator # "_i32.m $dst, $src0",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (sext vt_v:$src0),
        v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;
  }
}
/*
multiclass FSext_64<string operator, ValueType vt,
  bits<6> opcode, ValueType vt_v> {
  // FR - SS
  def SS : FR_OneOp_Unmasked_64<
    (outs GPR64:$dst),
    (ins GPR64:$src0),
    operator # "_i64 $dst, $src0",
    [(set i64:$dst, (sext_inreg i64:$src0, vt))],
    opcode,
    Fmt_S,
    Fmt_S>;

  // FR - VV unmasked
  def VV_U : FR_OneOp_Unmasked_64<
    (outs VR512L:$dst),
    (ins VR512L:$src0),
    operator # "_i64 $dst, $src0",
    [(set v8i64:$dst, (sext vt_v:$src0))],
    opcode,
    Fmt_V,
    Fmt_V>;

  let Constraints = "$dst = $oldvalue" in {
    // FR - VV masked
    def VV_M : FR_OneOp_Masked_64<
      (outs VR512L:$dst),
      (ins VR512L:$src0, VR512L:$oldvalue),
      operator # "_i64.m $dst, $src0",
      [(set v8i64:$dst, (int_npu_vector_mixi64 (sext vt_v:$src0),
        v8i64:$oldvalue))],
      opcode,
      Fmt_V,
      Fmt_V>;
  }
}
*/
//===----------------------------------------------------------------------===//
//  A set of multiclasses used to handle Loads and Stores
//===----------------------------------------------------------------------===//

// Scalar LOAD
multiclass FMLoadScalar_32<string suffix, PatFrag op_mem, PatFrag op_scratch, bits<6> opcode> {

  def _Mainmem : FM_Unmasked_Mainmem<
    (outs GPR32:$dstsrc), 
    (ins MEMri:$addr), 
    "load32" # suffix # " $dstsrc, $addr", 
    [(set i32:$dstsrc, (i32 (op_mem ADDRri:$addr)))], 
    opcode, 
    0>;

  def _Scratchpad : FM_Unmasked_Scratchpad<
    (outs GPR32:$dstsrc),
    (ins MEMri:$addr),
    "load32" # suffix # "_scratchpad $dstsrc, $addr",
    [(set i32:$dstsrc, (i32 (op_scratch ADDRri:$addr)))],
    opcode,
    0>;

}
/*
multiclass FMLoadScalar_64<string suffix, PatFrag op_mem, PatFrag op_scratch, bits<6> opcode> {

  def _Mainmem : FM_Unmasked_Mainmem<
    (outs GPR64:$dstsrc), 
    (ins MEMri:$addr), 
    "load64" # suffix # " $dstsrc, $addr", 
    [(set i64:$dstsrc, (i64 (op_mem ADDRri:$addr)))], 
    opcode, 
    1>;

  def _Scratchpad : FM_Unmasked_Scratchpad<
    (outs GPR64:$dstsrc),
    (ins MEMri:$addr),
    "load64" # suffix # "_scratchpad $dstsrc, $addr",
    [(set i64:$dstsrc, (i64 (op_scratch ADDRri:$addr)))],
    opcode,
    1>;
}
*/
// Scalar STORE
multiclass FMStoreScalar_32<string suffix, PatFrag op_mem, PatFrag op_scratch, bits<6> opcode> {

  def _Mainmem : FM_Unmasked_Mainmem<
    (outs),
    (ins GPR32:$dstsrc, MEMri:$addr),
    "store32" # suffix # " $dstsrc, $addr",
    [(op_mem i32:$dstsrc, ADDRri:$addr)],
    opcode, 
    0>;

  def _Scratchpad : FM_Unmasked_Scratchpad<
    (outs),
    (ins GPR32:$dstsrc, MEMri:$addr),
    "store32" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_scratch i32:$dstsrc, ADDRri:$addr)],
    opcode,
    0>{
        let hasSideEffects = 1;
        let mayStore = 1;
      }
}
/*
multiclass FMStoreScalar_64<string suffix, PatFrag op_mem, PatFrag op_scratch, bits<6> opcode> {

  def _Mainmem : FM_Unmasked_Mainmem<
    (outs), 
    (ins GPR64:$dstsrc, MEMri:$addr), 
    "store64" # suffix # " $dstsrc, $addr", 
    [(op_mem i64:$dstsrc, ADDRri:$addr)],
    opcode, 
    1>;

  def _Scratchpad : FM_Unmasked_Scratchpad<
    (outs),
    (ins GPR64:$dstsrc, MEMri:$addr),
    "store64" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_scratch i64:$dstsrc, ADDRri:$addr)],
    opcode,
    1>{
        let hasSideEffects = 1;
        let mayStore = 1;
      }
}
*/
// Vector LOAD
multiclass FMLoadVector_32<string suffix, PatFrag op_Umem, PatFrag op_Uscratch, 
                           Intrinsic op_Mmem, Intrinsic op_Mscratch, bits<6> opcode> {

  // main memory - unmasked - 32
  def Mainmem_U : FM_Unmasked_Mainmem_32<
    (outs VR512W:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # " $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Umem ADDRri:$addr))],
    opcode>;

  // scratchpad memory - unmasked - 32
  def Scratchpad_U : FM_Unmasked_Scratchpad_32<
    (outs VR512W:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # "_scratchpad $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Uscratch ADDRri:$addr))],
    opcode>;

  // main memory - masked - 32
  def Mainmem_M : FM_Masked_Mainmem_32<
    (outs VR512W:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # ".m $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Mmem ADDRri:$addr))],
    opcode>;

  // scratchpad memory - masked - 32
  def Scratchpad_M : FM_Masked_Scratchpad_32<
    (outs VR512W:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Mscratch ADDRri:$addr))],
    opcode>;
}
/*
multiclass FMLoadVector_64<string suffix, PatFrag op_Umem, PatFrag op_Uscratch, 
                           Intrinsic op_Mmem, Intrinsic op_Mscratch, bits<6> opcode> {
  
  // main memory - unmasked - 64
  def Mainmem_U : FM_Unmasked_Mainmem_64<
    (outs VR512L:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # " $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Umem ADDRri:$addr))],
    opcode>;
  // scratchpad memory - unmasked - 64
  def Scratchpad_U : FM_Unmasked_Scratchpad_64<
    (outs VR512L:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # "_scratchpad $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Uscratch ADDRri:$addr))],
    opcode>;
  // main memory - masked - 64
  def Mainmem_M : FM_Masked_Mainmem_64<
    (outs VR512L:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # ".m $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Mmem ADDRri:$addr))],
    opcode>;
  // scratchpad memory - masked - 64
  def Scratchpad_M : FM_Masked_Scratchpad_64<
    (outs VR512L:$dstsrc),
    (ins MEMri:$addr),
    "load" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Mscratch ADDRri:$addr))],
    opcode>;
}
*/
// Vector GATHER
multiclass FMGather_32<string suffix, Intrinsic op_Uscratch, 
                           Intrinsic op_Mscratch, bits<6> opcode> {
  
  // scratchpad memory - unmasked - 32
  def Scratchpad_U : FM_Unmasked_Scratchpad_32<
    (outs VR512W:$dstsrc),
    (ins V16MEMri:$addr),
    "loadg" # suffix # "_scratchpad $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Uscratch V16ADDRri:$addr))],
    opcode>;
  // scratchpad memory - masked - 32
  def Scratchpad_M : FM_Masked_Scratchpad_32<
    (outs VR512W:$dstsrc),
    (ins V16MEMri:$addr),
    "loadg" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(set v16i32:$dstsrc, (op_Mscratch V16ADDRri:$addr))],
    opcode>;
}
/*
multiclass FMGather_64<string suffix, Intrinsic op_Uscratch, 
                           Intrinsic op_Mscratch, bits<6> opcode> {
  
  // scratchpad memory - unmasked - 64
  def Scratchpad_U : FM_Unmasked_Scratchpad_64<
    (outs VR512L:$dstsrc),
    (ins V16MEMri:$addr),
    "loadg" # suffix # "_scratchpad $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Uscratch V16ADDRri:$addr))],
    opcode>;
  // scratchpad memory - masked - 64
  def Scratchpad_M : FM_Masked_Scratchpad_64<
    (outs VR512L:$dstsrc),
    (ins V16MEMri:$addr),
    "loadg" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(set v8i64:$dstsrc, (op_Mscratch V16ADDRri:$addr))],
    opcode>;
}
*/
// Vector STORE
multiclass FMStoreVector_32<string suffix, PatFrag op_Umem, PatFrag op_Uscratch, 
                           Intrinsic op_Mmem, Intrinsic op_Mscratch, bits<6> opcode> {

  // main memory - unmasked - 32
  def Mainmem_U : FM_Unmasked_Mainmem_32<
    (outs),
    (ins VR512W:$dstsrc, MEMri:$addr),
    "store" # suffix # " $dstsrc, $addr",
    [(op_Umem v16i32:$dstsrc, ADDRri:$addr)],
    opcode>;
  // scratchpad memory - unmasked - 32
  def Scratchpad_U : FM_Unmasked_Scratchpad_32<
    (outs),
    (ins VR512W:$dstsrc, MEMri:$addr),
    "store" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_Uscratch v16i32:$dstsrc, ADDRri:$addr)],
    opcode>;
  // main memory - masked - 32
  def Mainmem_M : FM_Masked_Mainmem_32<
    (outs),
    (ins VR512W:$dstsrc, MEMri:$addr),
    "store" # suffix # ".m $dstsrc, $addr",
    [(op_Mmem ADDRri:$addr, v16i32:$dstsrc)],
    opcode>;
  // scratchpad memory - masked - 32
  def Scratchpad_M : FM_Masked_Scratchpad_32<
    (outs),
    (ins VR512W:$dstsrc, MEMri:$addr),
    "store" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(op_Mscratch ADDRri:$addr, v16i32:$dstsrc)],
    opcode>;
}
/*
multiclass FMStoreVector_64<string suffix, PatFrag op_Umem, PatFrag op_Uscratch, 
                           Intrinsic op_Mmem, Intrinsic op_Mscratch, bits<6> opcode> {
 
  // main memory - unmasked - 64
  def Mainmem_U : FM_Unmasked_Mainmem_64<
    (outs),
    (ins VR512L:$dstsrc, MEMri:$addr),
    "store" # suffix # " $dstsrc, $addr",
    [(op_Umem v8i64:$dstsrc, ADDRri:$addr)],
    opcode>;
  // scratchpad memory - unmasked - 64
  def Scratchpad_U : FM_Unmasked_Scratchpad_64<
    (outs),
    (ins VR512L:$dstsrc, MEMri:$addr),
    "store" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_Uscratch v8i64:$dstsrc, ADDRri:$addr)],
    opcode>;
  // main memory - masked - 64
  def Mainmem_M : FM_Masked_Mainmem_64<
    (outs),
    (ins VR512L:$dstsrc, MEMri:$addr),
    "store" # suffix # ".m $dstsrc, $addr",
    [(op_Mmem ADDRri:$addr, v8i64:$dstsrc)],
    opcode>;
  // scratchpad memory - masked - 64
  def Scratchpad_M : FM_Masked_Scratchpad_64<
    (outs),
    (ins VR512L:$dstsrc, MEMri:$addr),
    "store" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(op_Mscratch ADDRri:$addr, v8i64:$dstsrc)],
    opcode>;
}
*/
// Vector SCATTER
multiclass FMScatter_32<string suffix, Intrinsic op_Uscratch, 
                           Intrinsic op_Mscratch, bits<6> opcode> {

  // scratchpad memory - unmasked - 32
  def Scratchpad_U : FM_Unmasked_Scratchpad_32<
    (outs),
    (ins VR512W:$dstsrc, V16MEMri:$addr),
    "stores" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_Uscratch V16ADDRri:$addr, v16i32:$dstsrc)],
    opcode>;

  // scratchpad memory - masked - 32
  def Scratchpad_M : FM_Masked_Scratchpad_32<
    (outs),
    (ins VR512W:$dstsrc, V16MEMri:$addr),
    "stores" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(op_Mscratch V16ADDRri:$addr, v16i32:$dstsrc)],
    opcode>;
}
/*
multiclass FMScatter_64<string suffix, Intrinsic op_Uscratch, 
                           Intrinsic op_Mscratch, bits<6> opcode> {

  // scratchpad memory - unmasked - 64
  def Scratchpad_U : FM_Unmasked_Scratchpad_64<
    (outs),
    (ins VR512L:$dstsrc, V16MEMri:$addr),
    "stores" # suffix # "_scratchpad $dstsrc, $addr",
    [(op_Uscratch V16ADDRri:$addr, v8i64:$dstsrc)],
    opcode>;

  // scratchpad memory - masked - 64
  def Scratchpad_M : FM_Masked_Scratchpad_64<
    (outs),
    (ins VR512L:$dstsrc, V16MEMri:$addr),
    "stores" # suffix # "_scratchpad.m $dstsrc, $addr",
    [(op_Mscratch V16ADDRri:$addr, v8i64:$dstsrc)],
    opcode>;
}
*/
//===----------------------------------------------------------------------===//
//  A set of multiclasses is used to handle Vector/Scalar 
//  Masked/Unmasked combinations
//  MOVEI operations
//===----------------------------------------------------------------------===//

multiclass FMOVEI_ALL<string operator, bits<3> opcode> {
  // SI
  def SI : FMOVEI<
    (outs GPR32:$dst),
    (ins SIMM16OP:$imm),
    operator # " $dst, $imm",
    [],//[(set i32:$dst, simm16:$imm)],
    opcode,
    Fmt_S,
    0>;

  // VI unmasked
  def VI_U : FMOVEI<
    (outs VR512W:$dst),
    (ins SIMM16OP:$imm),
    operator # " $dst, $imm",
    [],//[(set v16i32:$dst, (splat simm16:$imm))],
    opcode,
    Fmt_V,
    0>;

 let Constraints = "$dst = $oldvalue", Uses = [MR_REG] in {
    // VI masked
    def VI_M : FMOVEI<
      (outs VR512W:$dst),
      (ins SIMM16OP:$imm, VR512W:$oldvalue),
      operator # ".m $dst, $imm",
      [(set v16i32:$dst, (int_npu_vector_mixi32 (splat simm16:$imm), v16i32:$oldvalue))],
      opcode,
      Fmt_V,
      1>;
  }
}



//===----------------------------------------------------------------------===//
// Instruction class used to read/write special register through intrinics
// All these instructions are implemented using a move instruction
//===----------------------------------------------------------------------===//
let DecoderNamespace = "Read_SPR" in {
  class READ_SPR<SpReg reg, string operator, Intrinsic read_intr> :
                FR_OneOp_Unmasked_32<
                  (outs GPR32:$dst),
                  (ins),
                  operator # " $dst",
                  [(set i32:$dst, (read_intr))],
                  32,
                  Fmt_S,
                  Fmt_S>
  {
    bits<6> dst;

    let Inst{29-24} = 32; // opcode: move
    let Inst{23-18} = dst;
    let Inst{17-12} = reg.Register;
    let Inst{11-6} = 0;
  }
}
let DecoderNamespace = "Write_SPR" in {
  class WRITE_SPR<SpReg reg, string operator, Intrinsic read_intr> :
                FR_OneOp_Unmasked_32<
                  (outs),
                  (ins GPR32:$src),
                  operator # " $src",
                  [(read_intr i32:$src)],
                  32,
                  Fmt_S,
                  Fmt_S>
  {
    bits<6> src;

    let Inst{29-24} = 32; // opcode: move
    let Inst{23-18} = reg.Register;
    let Inst{17-12} = src;
    let Inst{11-6} = 0;
  }
}


//===----------------------------------------------------------------------===//
//  Pseudo-instructions for alternate assembly syntax (never used by codegen).
//  These are aliases that require C++ handling to convert to the target
//  instruction, while InstAliases can be handled directly by tblgen.
//===----------------------------------------------------------------------===//
class AsmPseudoInst<dag outs, dag ins, string asm>
  : InstNaplesPU<outs, ins, asm, []> {
  let isPseudo = 1;
}

class Pseudo<dag outs, dag ins, list<dag> pattern>
  : InstNaplesPU<outs, ins, "Pseudo", pattern>
{
  let isCodeGenOnly = 1;
  let isPseudo = 1;
  let Inst{31-0} = 0;
}

multiclass AtomicBinary<SDNode OpNode>
{
  def R : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$ptr, GPR32:$amt),
    [(set i32:$dst, (OpNode GPR32:$ptr, GPR32:$amt))]>;

  def I : Pseudo<
    (outs GPR32:$dst),
    (ins GPR32:$ptr, SIMM9OP:$amt),
    [(set i32:$dst, (OpNode GPR32:$ptr, simm9:$amt))]>;
}
