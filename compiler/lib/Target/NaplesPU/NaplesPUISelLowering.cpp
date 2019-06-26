//===-- NaplesPUISelLowering.cpp - NaplesPU DAG Lowering Implementation ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that NaplesPU uses to lower LLVM code
// into a selection DAG.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "naplespu-isel-lowering"

#include "NaplesPUISelLowering.h"
#include "NaplesPUMachineFunctionInfo.h"
#include "NaplesPUTargetMachine.h"
#include "NaplesPUTargetObjectFile.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include "NaplesPUGenCallingConv.inc"

const NaplesPUTargetLowering *
NaplesPUTargetLowering::create(const NaplesPUTargetMachine &TM,
                            const NaplesPUSubtarget &STI) {
  return new NaplesPUTargetLowering(TM, STI);
}

//===----------------------------------------------------------------------===//
// TargetLowering Implementation
//===----------------------------------------------------------------------===//

// The constructor caontains declarations about the register classes associated
// with the data types, the type of lowering action to perform given a specified 
// operation and type and other operations defined in TargetLowering.h
NaplesPUTargetLowering::NaplesPUTargetLowering(const TargetMachine &TM,
                                         const NaplesPUSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Data types supported
  // Scalar
  static const MVT ScalarIntDT[]   =  {MVT::i32};//, MVT::i64};
  static const MVT ScalarFloatDT[] =  {MVT::f32};//, MVT::f64};

  // Vector (512-bit wide)
  static const MVT VecIntDT[]   =   {MVT::v16i32};//, MVT::v8i64};
  static const MVT VecFloatDT[] =   {MVT::v16f32};//, MVT::v8f64};

  // Vector (< 512-bit wide)
  static const MVT VecLIntDT[]   = {MVT::v16i8, MVT::v16i16,
                                    MVT::v8i8, MVT::v8i16, MVT::v8i32}; 

  // Set up the register classes.
  // Scalar data types
  addRegisterClass(MVT::i32, &NaplesPU::GPR32RegClass);
  addRegisterClass(MVT::f32, &NaplesPU::GPR32RegClass);

//  addRegisterClass(MVT::i64, &NaplesPU::GPR64RegClass);
//  addRegisterClass(MVT::f64, &NaplesPU::GPR64RegClass);

  // Vector data types (512-bit wide)
  addRegisterClass(MVT::v16i32, &NaplesPU::VR512WRegClass);
  addRegisterClass(MVT::v16f32, &NaplesPU::VR512WRegClass);

//  addRegisterClass(MVT::v8i64, &NaplesPU::VR512LRegClass);
//  addRegisterClass(MVT::v8f64, &NaplesPU::VR512LRegClass);

  // Vector data types (< 512-bit wide)
  /*
  addRegisterClass(MVT::v16i8, &NaplesPU::VR512WRegClass);
  addRegisterClass(MVT::v16i16, &NaplesPU::VR512WRegClass);
  addRegisterClass(MVT::v8i8, &NaplesPU::VR512LRegClass);

  addRegisterClass(MVT::v8i16, &NaplesPU::VR512LRegClass);
  addRegisterClass(MVT::v8i32, &NaplesPU::VR512LRegClass);
  addRegisterClass(MVT::v8f32, &NaplesPU::VR512LRegClass);
*/
  
  // Definition of custom lowered operations.
  // Each custom operation is hadled by a different Lower function 
  // called by th LowerOperation method. By convention the functions
  // are called as lowerOperationName, with OperationName setted as the 
  // operation name used in ISDOpcode.h. 

  // VECTOR CUSTOM OPERATIONS
  static const unsigned VectCustomOps[] = {
      ISD::BUILD_VECTOR, ISD::VECTOR_SHUFFLE, ISD::SCALAR_TO_VECTOR};

  for (unsigned VectCustomOp : VectCustomOps)
  {
    for (MVT VT : VecIntDT)
    {
      setOperationAction(VectCustomOp, VT, Custom);
    }

    for (MVT VT : VecFloatDT)
    {
      setOperationAction(VectCustomOp, VT, Custom);
    }
  }

  setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v8i8, Custom);

/*
  //TODO: Controllare operazioni 
  static const unsigned VectPromotedOps[] = {
    // Integer arithmetic:
    ISD::ADD,     ISD::SUB,     ISD::MUL,     ISD::SDIV,    ISD::UDIV,
    ISD::SREM,    ISD::UREM,    ISD::SDIVREM, ISD::UDIVREM, ISD::ADDC,
    ISD::SUBC,    ISD::SADDO,   ISD::UADDO,   ISD::SSUBO,   ISD::USUBO,
    ISD::SMUL_LOHI,             ISD::UMUL_LOHI,
    // Logical/bit:
    ISD::AND,     ISD::OR,      ISD::XOR,     ISD::ROTL,    ISD::ROTR,
    ISD::CTPOP,   ISD::CTLZ,    ISD::CTTZ,
  };

  for (unsigned VectPromotedOp : VectPromotedOps) {
    for (MVT VT : {MVT::v16i8, MVT::v16i16}) {
      setOperationPromotedToType(VectPromotedOp, VT, MVT::v16i32);
    }

    for (MVT VT : {MVT::v8i8, MVT::v8i16, MVT::v8i32}) {
      setOperationPromotedToType(VectPromotedOp, VT, MVT::v8i64);
    }
  }
*/


  static const unsigned ScalarI32RemDivRemOps[] = {
      ISD::UREM, ISD::SREM, ISD::SDIVREM, ISD::UDIVREM,
  };

  static const unsigned ScalarI32DivOps[] = {
      ISD::SDIV, ISD::UDIV,
  };

  for (unsigned ScalarI32RemDivRemOp : ScalarI32RemDivRemOps ) {
      setOperationAction(ScalarI32RemDivRemOp, MVT::i32, Expand);
      setOperationAction(ScalarI32RemDivRemOp, MVT::v16i32, Expand);
      setOperationAction(ScalarI32RemDivRemOp, MVT::v16i8, Expand);
      setOperationAction(ScalarI32RemDivRemOp, MVT::v16i16, Expand);
  }
    

  for (unsigned ScalarI32DivOp : ScalarI32DivOps ) {
    
      setOperationAction(ScalarI32DivOp, MVT::i32, Custom);
      setOperationAction(ScalarI32DivOp, MVT::v16i32, Custom);
      setOperationAction(ScalarI32DivOp, MVT::v16i8,  Expand);
      setOperationAction(ScalarI32DivOp, MVT::v16i16, Expand);
  }
  // SCALAR CUSTOM OPERATIONS
  // I32
  static const unsigned ScalarI32CustomOps[] = {
      ISD::GlobalAddress, ISD::ConstantPool, ISD::BlockAddress, ISD::JumpTable,
      ISD::SELECT_CC, 
      ISD::CTTZ_ZERO_UNDEF, ISD::CTLZ_ZERO_UNDEF,
      ISD::UINT_TO_FP, ISD::FRAMEADDR, ISD::RETURNADDR,
  };

  for (unsigned ScalarI32CustomOp : ScalarI32CustomOps) {
    setOperationAction(ScalarI32CustomOp, MVT::i32, Custom);
  }

  // F32
  static const unsigned ScalarF32CustomOps[] = {
      ISD::GlobalAddress, ISD::ConstantPool, ISD::BlockAddress,
      ISD::SELECT_CC, ISD::SETCC,
      ISD::FNEG, ISD::FABS, 
  };

  for (unsigned ScalarF32CustomOp : ScalarF32CustomOps) {
    setOperationAction(ScalarF32CustomOp, MVT::f32, Custom);
  }



  // I64 - Commented - No Support
/*  static const unsigned ScalarI64CustomOps[] = {
    ISD::GlobalAddress, ISD::ConstantPool, ISD::BlockAddress,
    ISD::UINT_TO_FP,
    ISD::SELECT_CC,
    ISD::CTTZ_ZERO_UNDEF, ISD::CTLZ_ZERO_UNDEF,
  };

  for (unsigned ScalarI64CustomOp : ScalarI64CustomOps) {
    setOperationAction(ScalarI64CustomOp, MVT::i64, Custom);
  }

  // F64
  static const unsigned ScalarF64CustomOps[] = { 
    ISD::GlobalAddress, ISD::ConstantPool, ISD::BlockAddress,
    ISD::SELECT_CC,
    ISD::SETCC,
    ISD::FABS, ISD::FNEG, 
  };

  for (unsigned ScalarF64CustomOp : ScalarF64CustomOps) {
    setOperationAction(ScalarF64CustomOp, MVT::f64, Custom);
  }

// (FRANCESCO) Avoid 64-bit arithmetic ops 

  setOperationAction(ISD::ADD, MVT::i64, Custom);
  setOperationAction(ISD::SUB, MVT::i64, Expand);
  setOperationAction(ISD::MUL, MVT::i64, Expand);
  setOperationAction(ISD::SDIV, MVT::i64, Expand);
  setOperationAction(ISD::UDIV, MVT::i64, Expand);
  
  setOperationAction(ISD::FADD, MVT::f64, Expand);
  setOperationAction(ISD::FSUB, MVT::f64, Expand);
  setOperationAction(ISD::FMUL, MVT::f64, Expand);
  setOperationAction(ISD::FDIV, MVT::f64, Expand);

  setOperationAction(ISD::OR, MVT::i64, Custom);
 
*/

  setOperationAction(ISD::INTRINSIC_VOID, MVT::Other, Custom);

  setOperationAction(ISD::SETCC, MVT::v16i32, Custom);
  setOperationAction(ISD::SETCC, MVT::v16f32, Custom);
//  setOperationAction(ISD::SETCC, MVT::v8i64, Custom);
//  setOperationAction(ISD::SETCC, MVT::v8f64, Custom);

  setOperationAction(ISD::SELECT, MVT::v16i32, Custom);
  setOperationAction(ISD::SELECT, MVT::v16f32, Custom);
//  setOperationAction(ISD::SELECT, MVT::v8i64, Custom);
//  setOperationAction(ISD::SELECT, MVT::v8f64, Custom);

  setOperationAction(ISD::SELECT_CC, MVT::v16i32, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::v16f32, Custom);
//  setOperationAction(ISD::SELECT_CC, MVT::v8i64, Custom);
//  setOperationAction(ISD::SELECT_CC, MVT::v8f64, Custom);

  setOperationAction(ISD::VSELECT, MVT::v16i32, Custom);
  setOperationAction(ISD::VSELECT, MVT::v16f32, Custom);
//  setOperationAction(ISD::VSELECT, MVT::v8i64, Custom);
//  setOperationAction(ISD::VSELECT, MVT::v8f64, Custom);
  
  setOperationAction(ISD::FNEG, MVT::v16f32, Custom);
//  setOperationAction(ISD::FNEG, MVT::v8f64, Custom);

  setOperationAction(ISD::UINT_TO_FP, MVT::v16i32, Custom);
//  setOperationAction(ISD::UINT_TO_FP, MVT::v8i64, Custom);

  setOperationAction(ISD::FABS, MVT::v16f32, Custom);
//  setOperationAction(ISD::FABS, MVT::v8f64, Custom);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::v16i1, Custom);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::v8i1, Custom);

  setCondCodeAction(ISD::SETUEQ, MVT::v16f32, Custom);
  setCondCodeAction(ISD::SETUNE, MVT::v16f32, Custom);
  setCondCodeAction(ISD::SETUGT, MVT::v16f32, Custom);
  setCondCodeAction(ISD::SETUGE, MVT::v16f32, Custom);
  setCondCodeAction(ISD::SETULT, MVT::v16f32, Custom);
  setCondCodeAction(ISD::SETULE, MVT::v16f32, Custom);


  // EXPANDED OPERATIONS
  // SCALAR INTEGERS
  static const unsigned ScalarIntExpOps[] = {
    ISD::CTPOP, ISD::BR_CC, ISD::BRCOND, ISD::SELECT, ISD::ROTL, 
    ISD::ROTR, ISD::UDIVREM, ISD::SDIVREM, ISD::FP_TO_UINT, ISD::BSWAP,
    ISD::ATOMIC_LOAD, ISD::ATOMIC_STORE, 
  };

  for (unsigned ScalarIntExpOp : ScalarIntExpOps) {
    for (MVT VT : ScalarIntDT) {
      setOperationAction(ScalarIntExpOp, VT, Expand);
    }
  }

  // SCALAR FLOAT
  static const unsigned ScalarFloatExpOps[] = {
    ISD::BR_CC, ISD::BRCOND, ISD::SELECT, ISD::FCOPYSIGN, ISD::FFLOOR,
    
  };

  for (unsigned ScalarFloatExpOp : ScalarFloatExpOps) {
    for (MVT VT : ScalarFloatDT) {
      setOperationAction(ScalarFloatExpOp, VT, Expand);
    }
  }

// TODO: (Catello) Find a way to improve division and reminder operations performance
  // VECTOR INTEGERS (512-BIT WIDE)
  static const unsigned VecIntExpOps[] = {
    ISD::ROTL, ISD::ROTR, ISD::FP_TO_UINT, 
    ISD::UDIVREM, ISD::SDIVREM, 
  };

  for (unsigned VecIntExpOp : VecIntExpOps) {
    for (MVT VT :VecIntDT) {
      setOperationAction(VecIntExpOp, VT, Expand);
    }
  }

  // VECTOR FLOAT (512-BIT WIDE)
  static const unsigned VecFloatExpOps[] = {
    ISD::FFLOOR, ISD::FREM,
  };

  for (unsigned VecFloatExpOp : VecFloatExpOps) {
    for (MVT VT :VecFloatDT) {
      setOperationAction(VecFloatExpOp, VT, Expand);
    }
  }


  setOperationAction(ISD::FREM, MVT::f32, Expand);
  //setOperationAction(ISD::FREM, MVT::f64, Expand);


  setOperationAction(ISD::BR_JT, MVT::Other, Expand);


  // SMUL_LOHI/UMUL_LOHI - Multiply two integers of type iN, producing a signed/unsigned 
  // value of type i[2*N], and return the full value as two results, each of type iN. 
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  
  //Allocate some number of bytes on the stack aligned to a specified boundary. 
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32, Expand);
  setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
  setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);

  // Carry-setting and Carry-using nodes for multiple precision addition and subtraction.
  setOperationAction(ISD::ADDC, MVT::i32, Custom);
  setOperationAction(ISD::ADDE, MVT::i32, Expand);
  setOperationAction(ISD::SUBC, MVT::i32, Expand);
  setOperationAction(ISD::SUBE, MVT::i32, Expand);
  //Expanded integer shift operations.
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);

  setOperationAction(ISD::VAARG, MVT::Other, Expand);
  setOperationAction(ISD::VACOPY, MVT::Other, Expand);
  setOperationAction(ISD::VAEND, MVT::Other, Expand);
  
  //Used for the FrameIndex for start of variable arguments area
  setOperationAction(ISD::VASTART, MVT::Other, Custom);

  // Library calls
  // The signature is defined in the '''InitLibcallNames''' in 
  // "/compiler/lib/CodeGen/TargetLoweringBase.cpp".
  setOperationAction(ISD::FSQRT, MVT::f32, Expand); // sqrtf
  setOperationAction(ISD::FSIN, MVT::f32, Expand);  // sinf
  setOperationAction(ISD::FCOS, MVT::f32, Expand);  // cosf
  setOperationAction(ISD::FSINCOS, MVT::f32, Expand);

//  setOperationAction(ISD::FSQRT, MVT::f64, Expand); // sqrtf
//  setOperationAction(ISD::FSIN, MVT::f64, Expand);  // sinf
//  setOperationAction(ISD::FCOS, MVT::f64, Expand);  // cosf
//  setOperationAction(ISD::FSINCOS, MVT::f64, Expand);
  // -----
  
  // This is used to expand the extload and truncstore betwen f32 and f64
  // to those between i32 and i64
//  setLoadExtAction(ISD::EXTLOAD, MVT::f64, MVT::f32, Expand);
//  setTruncStoreAction(MVT::f64, MVT::f32, Expand);

  setStackPointerRegisterToSaveRestore(NaplesPU::SP_REG);
  setMinFunctionAlignment(2);
  //Since there is no CMOV, tells the code generator not to expand 
  //operations into sequences that use the select operations if possible. 
  //setSelectIsExpensive();

  computeRegisterProperties(Subtarget.getRegisterInfo());

  //Used to expand a vector comparison result into a vector
  setOperationAction(ISD::ConstantFP, MVT::f32, Legal);
//  setOperationAction(ISD::ConstantFP, MVT::f64, Legal);

 /*
  setOperationAction(, MVT::v16f32, Custom);
  setOperationAction(ISD::BUILD_VECTOR, MVT::v16i32, Custom);
  setOperationAction(ISD::BUILD_VECTOR, MVT::v8f64, Custom);
  setOperationAction(ISD::BUILD_VECTOR, MVT::v8i64, Custom);
  //insert a scalar into a vector
  //setOperationAction(ISD::INSERT_VECTOR_ELT, MVT::v16f32, Custom);
  //setOperationAction(ISD::INSERT_VECTOR_ELT, MVT::v16i32, Custom);
  setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v16i32, Custom);
  setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v16f32, Custom);
  setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v8i64, Custom);
  setOperationAction(ISD::VECTOR_SHUFFLE, MVT::v8f64, Custom);
  setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v16i32, Custom);
  setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v16f32, Custom);
  setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v8i64, Custom);
  setOperationAction(ISD::SCALAR_TO_VECTOR, MVT::v8f64, Custom);
 */

 /*
  setCondCodeAction(ISD::SETO, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUO, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUEQ, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUNE, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUGT, MVT::f32, Custom);
  setCondCodeAction(ISD::SETUGE, MVT::f32, Custom);
  setCondCodeAction(ISD::SETULT, MVT::f32, Custom);
  setCondCodeAction(ISD::SETULE, MVT::f32, Custom);
 */
  
 /* //FCOPYSIGN(X, Y) - Return the value of X with the sign of Y. 

  // Hardware does not have an integer divider, so convert these to
  // library calls
  setOperationAction(ISD::UDIV, MVT::i32, Expand); // __udivsi3
  setOperationAction(ISD::UREM, MVT::i32, Expand); // __umodsi3
  setOperationAction(ISD::SDIV, MVT::i32, Expand); // __divsi3
  setOperationAction(ISD::SREM, MVT::i32, Expand); // __modsi3
  
  setOperationAction(ISD::SDIV, MVT::i64, Expand); // __divdi3
  setOperationAction(ISD::UDIV, MVT::i64, Expand); // __udivdi3
  setOperationAction(ISD::UREM, MVT::i64, Expand); // __umoddi3
  setOperationAction(ISD::SREM, MVT::i64, Expand); // __moddi3
 */
  
}

const char *NaplesPUTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case NaplesPUISD::CALL:
    return "NaplesPUISD::CALL";
  case NaplesPUISD::RET_FLAG:
    return "NaplesPUISD::RET_FLAG";
  case NaplesPUISD::SPLAT:
    return "NaplesPUISD::SPLAT";
  case NaplesPUISD::SEL_COND_RESULT:
    return "NaplesPUISD::SEL_COND_RESULT";
  case NaplesPUISD::LEAH:
    return "NaplesPUISD::LEAH";
  case NaplesPUISD::LEAL:
    return "NaplesPUISD::LEAL";
  case NaplesPUISD::FGT:
    return "NaplesPUISD::FGT";
  case NaplesPUISD::FGE:
    return "NaplesPUISD::FGE";
  case NaplesPUISD::FLT:
    return "NaplesPUISD::FLT";
  case NaplesPUISD::FLE:
    return "NaplesPUISD::FLE";
  case NaplesPUISD::FEQ:
    return "NaplesPUISD::FEQ";
  case NaplesPUISD::FNE:
    return "NaplesPUISD::FNE";
  default:
    return nullptr;
  }
}

//===----------------------------------------------------------------------===//
// Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
NaplesPUTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();

  // CCValAssign - Represent assignment of one arg/retval to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot (which registers are already assigned and which
  // stack slots are used).
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analyze return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_NaplesPU32);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);

    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // If the function returns a struct, copy the SRetReturnReg to S0
  if (MF.getFunction().hasStructRetAttr()) {

    NaplesPUMachineFunctionInfo *VFI = MF.getInfo<NaplesPUMachineFunctionInfo>();
	//return the virtual register into which the sret argument is passed
    unsigned Reg = VFI->getSRetReturnReg();
    if (!Reg)
      llvm_unreachable("sret virtual register not created in the entry block");

    SDValue Val =
        DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
    Chain = DAG.getCopyToReg(Chain, DL, NaplesPU::S0, Val, Flag);
    Flag = Chain.getValue(1);
    RetOps.push_back(
        DAG.getRegister(NaplesPU::S0, getPointerTy(DAG.getDataLayout())));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  return DAG.getNode(NaplesPUISD::RET_FLAG, DL, MVT::Other, RetOps);
}

SDValue NaplesPUTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL, 
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  // Analyze operands of the call, assigning locations to each operand.
  // NaplesPUCallingConv.td will auto-generate CC_NaplesPU32, which
  // knows how to handle operands (what go in registers vs. stack, etc).
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_NaplesPU32);

  // Walk through each parameter and push into InVals
  int ParamEndOffset = 0;
  for (const auto &VA : ArgLocs) {
    if (VA.isRegLoc()) {
      // Argument is in register
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i32 || RegVT == MVT::f32)
        RC = &NaplesPU::GPR32RegClass;
  //    else if (RegVT == MVT::i64 || RegVT == MVT::f64)
//        RC = &NaplesPU::GPR64RegClass;
      else if (RegVT == MVT::v16i32 || RegVT == MVT::v16f32)/* || 
				RegVT == MVT::v16i8 || RegVT == MVT::v16i16)*/
        RC = &NaplesPU::VR512WRegClass;
/*      else if (RegVT == MVT::v8i64 || RegVT == MVT::v8f64 )/*||
                RegVT == MVT::v8i8 || RegVT == MVT::v8i16 ||
                RegVT == MVT::v8i32 || RegVT == MVT::v8f32)*/
//        RC = &NaplesPU::VR512LRegClass;
      else
        llvm_unreachable("Unsupported formal argument Type");

      unsigned VReg = RegInfo.createVirtualRegister(RC);
      MF.getRegInfo().addLiveIn(VA.getLocReg(), VReg);
      SDValue Arg = DAG.getCopyFromReg(Chain, DL, VReg, VA.getLocVT());
      InVals.push_back(Arg);
      continue;
    }

    // The registers are exhausted. This argument was passed on the stack.
    assert(VA.isMemLoc());
    int ParamSize = VA.getValVT().getSizeInBits() / 8;
    int ParamOffset = VA.getLocMemOffset();
    int FI = MF.getFrameInfo().CreateFixedObject(ParamSize, ParamOffset, true);
    ParamEndOffset = ParamOffset + ParamSize;

    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue Load;
	//TODO: check the types
    if (VA.getValVT() == MVT::i32 || VA.getValVT() == MVT::f32 ||
  //      VA.getValVT() == MVT::i64 || VA.getValVT() == MVT::f64 ||
        VA.getValVT() == MVT::v16i32 || VA.getValVT() == MVT::v16f32)// ||
//	      VA.getValVT() == MVT::v8i64 || VA.getValVT() == MVT::v8f64) 
{
      // Primitive Types are loaded directly from the stack
      Load = DAG.getLoad(VA.getValVT(), DL, Chain, FIPtr, MachinePointerInfo());
    } else {
      // This is a smaller Type (char, etc).  Sign extend.
      ISD::LoadExtType LoadOp = ISD::SEXTLOAD;
      unsigned Offset = 4 - std::max(1U, VA.getValVT().getSizeInBits() / 8);
      FIPtr = DAG.getNode(ISD::ADD, DL, MVT::i32, FIPtr,
                          DAG.getConstant(Offset, DL, MVT::i32));
      Load = DAG.getExtLoad(LoadOp, DL, MVT::i32, Chain, FIPtr,
                            MachinePointerInfo(), VA.getValVT());
      Load = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), Load);
    }

    InVals.push_back(Load);
  }

  NaplesPUMachineFunctionInfo *VFI = MF.getInfo<NaplesPUMachineFunctionInfo>();

  if (isVarArg) {
    // Create a dummy object where the first parameter would start.  This will
    // be used
    // later to determine the start address of variable arguments.
    int FirstVarArg =
        MF.getFrameInfo().CreateFixedObject(4, ParamEndOffset, false);
    VFI->setVarArgsFrameIndex(FirstVarArg);
  }

  if (MF.getFunction().hasStructRetAttr()) {
    // When a function returns a structure, the address of the return value
    // is placed in the first physical register.
    unsigned Reg = VFI->getSRetReturnReg();
    if (!Reg) {
      Reg = MF.getRegInfo().createVirtualRegister(&NaplesPU::GPR32RegClass);
      VFI->setSRetReturnReg(Reg);
    }

    SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[0]);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
  }

  return Chain;
}

// Generate code to call a function
SDValue NaplesPUTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG 			              = CLI.DAG;
  SDLoc &DL 				                    = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
  SmallVector<SDValue, 32> &OutVals 	  = CLI.OutVals;
  SmallVector<ISD::InputArg, 32> &Ins 	= CLI.Ins;
  SDValue Chain 			                  = CLI.Chain;
  SDValue Callee 			                  = CLI.Callee;
  CallingConv::ID CallConv 		          = CLI.CallConv;
  bool isVarArg 			                  = CLI.IsVarArg;

  // NaplesPU target does not yet support tail call optimization.
  CLI.IsTailCall = false;

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  const TargetFrameLowering *TFL = MF.getSubtarget().getFrameLowering();

  // Analyze operands of the call, assigning locations to each operand.
  // NaplesPUCallingConv.td will auto-generate CC_NaplesPU32, which knows how to
  // handle operands (what go in registers vs. stack, etc).
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_NaplesPU32);

  // Get the size of the outgoing arguments stack space requirement.
  // We always keep the stack pointer 64 byte aligned so we can use block
  // loads/stores for vector arguments
  unsigned ArgsSize =
      alignTo(CCInfo.getNextStackOffset(), TFL->getStackAlignment());

  // Create local copies for all arguments that are passed by value
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    unsigned Align = Flags.getByValAlign();

	if (Size > 0U) {
		int FI = MFI.CreateStackObject(Size, Align, false);
		SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
		SDValue SizeNode = DAG.getConstant(Size, DL, MVT::i32);
		Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Align,
							  false,        // isVolatile,
							  (Size <= 32), // AlwaysInline if size <= 32
							  false, 		// isTailCall
							  MachinePointerInfo(), MachinePointerInfo());

		ByValArgs.push_back(FIPtr);
	}
	//TODO: controllare
	else {
		SDValue nullVal;
		ByValArgs.push_back(nullVal);
    }
  }

  // CALLSEQ_START will decrement the stack to reserve space
  Chain = DAG.getCALLSEQ_START(Chain, ArgsSize, 0, DL);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // Walk through arguments, storing each one to the proper palce
  for (unsigned i = 0, realArgIdx = 0, byvalArgIdx = 0, e = ArgLocs.size();
       i != e; ++i, ++realArgIdx) {

    CCValAssign &VA = ArgLocs[i];
    SDValue Arg = OutVals[realArgIdx];

    ISD::ArgFlagsTy Flags = Outs[realArgIdx].Flags;

    // Use local copy we created above if this is passed by value
    if (Flags.isByVal())
      Arg = ByValArgs[byvalArgIdx++];

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
    case CCValAssign::Full:
      break;

    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, VA.getLocVT(), Arg);
      break;

    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, VA.getLocVT(), Arg);
      break;

    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, DL, VA.getLocVT(), Arg);
      break;

    case CCValAssign::BCvt:
      Arg = DAG.getNode(ISD::BITCAST, DL, VA.getLocVT(), Arg);
      break;

    default:
      llvm_unreachable("Unknown loc info!");
    }

    // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
      continue;
    }

    // This needs to be pushed on the stack
    assert(VA.isMemLoc());

    // Create a store off the stack pointer for this argument.
    SDValue StackPtr = DAG.getRegister(NaplesPU::SP_REG, MVT::i32);
    SDValue PtrOff = DAG.getIntPtrConstant(VA.getLocMemOffset(), DL);
    PtrOff = DAG.getNode(ISD::ADD, DL, MVT::i32, StackPtr, PtrOff);
    MemOpChains.push_back(
		DAG.getStore(Chain, DL, Arg, PtrOff, MachinePointerInfo()));
  }

  // Emit all stores, make sure the occur before any copies into physregs.
  if (!MemOpChains.empty()) {
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain
  // and flag operands which copy the outgoing args into registers. The InFlag
  // in necessary since all emitted instructions must be stuck together.
  SDValue InFlag;
  for (const auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Get the function address.
  // If the callee is a GlobalAddress node (quite common, every direct call is)
  // turn it into a TargetGlobalAddress node so that legalize doesn't hack it.
  // Likewise ExternalSymbol -> TargetExternalSymbol.
  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee))
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL, MVT::i32);
  else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee))
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);

  // Returns a chain & a flag for retval copy to use
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  for (const auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(MF, CLI.CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(CLI.DAG.getRegisterMask(Mask));

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  Chain = DAG.getNode(NaplesPUISD::CALL, DL, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(ArgsSize, DL, true),
                             DAG.getIntPtrConstant(0, DL, true), InFlag, DL);
  InFlag = Chain.getValue(1);

  // The call has returned, handle return values
  SmallVector<CCValAssign, 16> RVLocs;
      CCState RVInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                     *DAG.getContext());

      RVInfo.AnalyzeCallResult(Ins, RetCC_NaplesPU32);

      // Copy all of the result registers out of their specified physreg.
      for (const auto &Loc : RVLocs) {
        Chain =
            DAG.getCopyFromReg(Chain, DL, Loc.getLocReg(), Loc.getValVT(), InFlag)
                .getValue(1);
        InFlag = Chain.getValue(2);
        InVals.push_back(Chain.getValue(0));
      }

      return Chain;
    }


    //handle 64 bit operands that are split and passed in two 32bit registers
    /*static bool CC_NaplesPU_Assign_Split_64(unsigned &ValNo, MVT &ValVT,
                                         MVT &LocVT, CCValAssign::LocInfo &LocInfo,
                                         ISD::ArgFlagsTy &ArgFlags, CCState &State)
    {
      static const MCPhysReg RegList[] = {
        NaplesPU::S0, NaplesPU::S1, NaplesPU::S2, NaplesPU::S3, NaplesPU::S4, NaplesPU::S5, NaplesPU::S6, NaplesPU::S7
      };
      // Try to get first reg.
      if (unsigned Reg = State.AllocateReg(RegList)) {
        State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
      } else {
        // Assign whole thing in stack.
        State.addLoc(CCValAssign::getCustomMem(ValNo, ValVT,
                                               State.AllocateStack(8,4),
                                               LocVT, LocInfo));
        return true;
      }

      // Try to get second reg.
      if (unsigned Reg = State.AllocateReg(RegList))
        State.addLoc(CCValAssign::getCustomReg(ValNo, ValVT, Reg, LocVT, LocInfo));
      else
        State.addLoc(CCValAssign::getCustomMem(ValNo, ValVT,
                                               State.AllocateStack(4,4),
                                               LocVT, LocInfo));
      return true;
    }*/


    //===----------------------------------------------------------------------===//
    // END - Calling Convention Implementation
    //===----------------------------------------------------------------------===//

    //===----------------------------------------------------------------------===//
    //              NaplesPU Custom Inserted MachineNodes
    //===----------------------------------------------------------------------===//

    // Return the SPLAT Node if the spalt operation is possible given the Destination
    // ValueType.
    SDValue CheckSplat(SDValue OpSplat, SDValue OpScalar, SelectionDAG &DAG) {
      SDLoc DL(OpSplat);

      EVT SplatVT = OpSplat.getValueType();
      //EVT OpVT = OpScalar.getValueType();

      SDValue Operand;
      SDValue Res;

      if(SplatVT == MVT::v16i32 || /*SplatVT == MVT::v8i64 ||*/ SplatVT == MVT::v16f32 /*|| SplatVT == MVT::v8f64*/)
        return DAG.getNode(NaplesPUISD::SPLAT, DL, SplatVT, OpScalar);

    /*
      if(SplatVT == MVT::v16i8 || SplatVT == MVT::v16i16) {

        Operand = DAG.getNode(ISD::SIGN_EXTEND, DL, MVT::i32, OpScalar,
                                                            DAG.getValueType(OpVT));
        Res = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v16i32, Operand);

      } else if(SplatVT == MVT::v8i8 || SplatVT == MVT::v8i16 || SplatVT == MVT::v8i32){

        Operand = DAG.getNode(ISD::SIGN_EXTEND, DL, MVT::i64, OpScalar,
                                                            DAG.getValueType(OpVT));
        Res = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v8i64, Operand);
      } else if(SplatVT == MVT::v8f32){
        Operand = DAG.getNode(ISD::FP_EXTEND, DL, MVT::f64, OpScalar,
                                                            DAG.getValueType(OpVT));
        Res = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v8f64, Operand);
      } else{*/
        assert("CheckSplat: type not supported!");
     // }

      return Res;
    }

    // Utility functions used in LowerGlobalAddress, LowerConstantPool, LowerBlockAddress,
    // and LowerJumpTable to get the proper Node.
    SDValue NaplesPUTargetLowering::getTargetNode(const GlobalAddressSDNode *N, EVT Ty,
                                               SelectionDAG &DAG) const {
      return DAG.getTargetGlobalAddress(N->getGlobal(), SDLoc(N), Ty);
    }

    SDValue NaplesPUTargetLowering::getTargetNode(const ConstantPoolSDNode *N, EVT Ty,
                                               SelectionDAG &DAG) const {
      return DAG.getTargetConstantPool(N->getConstVal(), Ty, N->getAlignment(),
                                       N->getOffset());
    }

    SDValue NaplesPUTargetLowering::getTargetNode(const BlockAddressSDNode *N, EVT Ty,
                                               SelectionDAG &DAG) const {
      return DAG.getTargetBlockAddress(N->getBlockAddress(), Ty);
    }

    SDValue NaplesPUTargetLowering::getTargetNode(const JumpTableSDNode *N, EVT Ty,
                                               SelectionDAG &DAG) const {
      return DAG.getTargetJumpTable(N->getIndex(), Ty);
    }

    template <class NodeTy>
    SDValue NaplesPUTargetLowering::getAddr(const NodeTy *N, SelectionDAG &DAG) const {
      SDLoc DL(N);
      EVT Ty = getPointerTy(DAG.getDataLayout());

      SDValue Hi = getTargetNode(N, Ty, DAG);
      SDValue Lo = getTargetNode(N, Ty, DAG);
      SDValue MoveHi = DAG.getNode(NaplesPUISD::LEAH, DL, MVT::i32, Hi);
      return DAG.getNode(NaplesPUISD::LEAL, DL, MVT::i32, MoveHi, Lo);
    }

    // Global addresses are stored in the per-function constant pool.
    SDValue NaplesPUTargetLowering::LowerGlobalAddress(SDValue Op,
                                                    SelectionDAG &DAG) const {
      const GlobalAddressSDNode *GV = cast<GlobalAddressSDNode>(Op);

      /*
      SDValue CPIdx = DAG.getTargetConstantPool(GV, MVT::i32);
      return DAG.getLoad(
          MVT::i32, DL, DAG.getEntryNode(), CPIdx,
          MachinePointerInfo::getConstantPool(DAG.getMachineFunction()), 4);
      */
      return getAddr(GV, DAG);

    }

    SDValue NaplesPUTargetLowering::LowerConstantPool(SDValue Op,
                                                   SelectionDAG &DAG) const {
      //SDLoc DL(Op);
      //EVT PtrVT = Op.getValueType();
      ConstantPoolSDNode *CP = cast<ConstantPoolSDNode>(Op);
      
      return getAddr(CP, DAG);
      /*
      SDValue Res;

      errs()<<"LowerConstantPool\n";
      Op.getNode()->print(errs()); errs()<<"\n";
      if (CP->isMachineConstantPoolEntry()) {
        Res = DAG.getTargetConstantPool(CP->getMachineCPVal(), PtrVT,
                                        CP->getAlignment());
      } else {
        Res = DAG.getTargetConstantPool(CP->getConstVal(), PtrVT, CP->getAlignment());
      }

      return Res;
      */
    }


    SDValue NaplesPUTargetLowering::LowerBlockAddress(SDValue Op,
                                                   SelectionDAG &DAG) const {
      //SDLoc DL(Op);
      const BlockAddressSDNode *BA = cast<BlockAddressSDNode>(Op);
      return getAddr(BA, DAG);
      /*
      SDValue CPIdx = DAG.getTargetConstantPool(BA, MVT::i32);
      return DAG.getLoad(
          MVT::i32, DL, DAG.getEntryNode(), CPIdx,
          MachinePointerInfo::getConstantPool(DAG.getMachineFunction()), 4);
          */
    }

    SDValue NaplesPUTargetLowering::LowerJumpTable(SDValue Op,
                                                SelectionDAG &DAG) const {
      JumpTableSDNode *N = cast<JumpTableSDNode>(Op);
      return getAddr(N, DAG);
    }

    // Used to lower the write_maskvxiy intrinsics. These are lowered using the 
    // createmask followed by write_mask_reg. This is done because the write_maskvxiy
    // intrinsics take a vxiy input operand, according to the opencl vector comparison result,
    // while the write_mask_reg requires a 32-bit value. So the vector input is translated first
    // in an integer value using the createmaskvxiy intrinsic, which result is given to
    // the write_mask_reg intrinsic.
    SDValue NaplesPUTargetLowering::LowerINTRINSIC_VOID(SDValue Op,
                                                SelectionDAG &DAG) const {

      SDValue Chain = Op.getOperand(0);
      unsigned IntNo = cast<ConstantSDNode>(Op.getOperand(1))->getZExtValue();
      
      if (IntNo == Intrinsic::npu_write_mask_regv16i32) {
        SDLoc DL(Op);
        SDValue VMask = Op.getOperand(2);

        SDValue SMask = DAG.getNode(ISD::INTRINSIC_W_CHAIN, DL, MVT::i32, Chain,
                          DAG.getConstant(Intrinsic::npu_createmaskv16i32, DL, MVT::i32),
                          VMask);
        return DAG.getNode(ISD::INTRINSIC_VOID, DL, MVT::Other,
                          Chain, DAG.getConstant(Intrinsic::npu_write_mask_reg, DL, MVT::i32),
                          SMask);
      }
    /*  if (IntNo == Intrinsic::npu_write_mask_regv8i64) {
        SDLoc DL(Op);
        SDValue VMask = Op.getOperand(2);

        SDValue SMask = DAG.getNode(ISD::INTRINSIC_W_CHAIN, DL, MVT::i32, Chain,
                          DAG.getConstant(Intrinsic::npu_createmaskv8i64, DL, MVT::i32),
                          VMask);
        return DAG.getNode(ISD::INTRINSIC_VOID, DL, MVT::Other,
                          Chain, DAG.getConstant(Intrinsic::npu_write_mask_reg, DL, MVT::i32),
                          SMask);
      }
      */
      return SDValue();
    }

    /*
     * SDIV Lowering:
     * realized through FPU in the following steps:
     *  - ITOF -> to get FP operands
     *  - FDIV -> to get the result
     *  - FTOI -> to extract the integer value
     *
     */
    SDValue NaplesPUTargetLowering::LowerXDIV(SDValue Op, SelectionDAG &DAG) const {
   SDLoc dl(Op);

   unsigned itof_opcode, ftoi_opcode;

   if(Op.getOpcode() == ISD::SDIV){
       itof_opcode = ISD::SINT_TO_FP;
       ftoi_opcode = ISD::FP_TO_SINT;
   } else {
       itof_opcode = ISD::UINT_TO_FP;
       ftoi_opcode = ISD::FP_TO_UINT;
   }

   SDValue op1 = Op.getOperand(0);
   SDValue op2 = Op.getOperand(1);
    
   //Converting operands to floating-point
   SDValue op1_fp = DAG.getNode(itof_opcode, dl, MVT::f32, op1);
   SDValue op2_fp = DAG.getNode(itof_opcode, dl, MVT::f32, op2);

   //Calculating FP Result
   SDValue res_fp = DAG.getNode(ISD::FDIV, dl, MVT::f32, op1_fp, op2_fp);

   //Conveting back to sint
   SDValue res_si = DAG.getNode(ftoi_opcode, dl, MVT::i32, res_fp);

   return res_si;

}

SDValue NaplesPUTargetLowering::LowerXDIV_VV(SDValue Op, SelectionDAG &DAG) const {
   SDLoc dl(Op);

   unsigned itof_opcode, ftoi_opcode;

   if(Op.getOpcode() == ISD::SDIV){
       itof_opcode = ISD::SINT_TO_FP;
       ftoi_opcode = ISD::FP_TO_SINT;
   } else {
       itof_opcode = ISD::UINT_TO_FP;
       ftoi_opcode = ISD::FP_TO_UINT;
   }

   SDValue op1 = Op.getOperand(0);
   SDValue op2 = Op.getOperand(1);

   //Converting operands to floating-point
   SDValue op1_fp = DAG.getNode(itof_opcode, dl, MVT::v16f32, op1);
   SDValue op2_fp = DAG.getNode(itof_opcode, dl, MVT::v16f32, op2);

   //Calculating FP Result
   SDValue res_fp = DAG.getNode(ISD::FDIV, dl, MVT::v16f32, op1_fp, op2_fp);

   //Conveting back to sint
   SDValue res_si = DAG.getNode(ftoi_opcode, dl, MVT::v16i32, res_fp);

   return res_si;

}
/*
SDValue NaplesPUTargetLowering::LowerADD_ADDC(SDValue Op, SelectionDAG &DAG) const {
//Se non è 64 bit ritorno
	if(Op.getValueType() != MVT::i64) return Op;	

	SDLoc dl(Op);
	
	SDValue op1 = Op.getOperand(0);
	SDValue op2 = Op.getOperand(1);

	SDValue op1Lo = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op1);
	SDValue op1Hi = DAG.getNode(ISD::SRL, dl, MVT::i64, op1, DAG.getConstant(32, dl, MVT::i64));
	op1Hi = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op1Hi);
	
	SDValue op2Lo = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op2);
	SDValue op2Hi = DAG.getNode(ISD::SRL, dl, MVT::i64, op2, DAG.getConstant(32, dl, MVT::i64));
	op2Hi = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op2Hi);
	
	SDValue LoAdd = DAG.getNode(ISD::ADDC, dl, MVT::i32, op1Lo, op2Lo);
//Hi Add with no Carry coming from LoAdd
	SDValue HiAddNc = DAG.getNode(ISD::ADDC, dl, MVT::i32, op1Hi, op2Hi);
// Calculate Carry from Lower Add
	SDValue CarryFromLoAdd = LoAdd.getValue(1);
// Add Lower Add carry to Hi level Add
	SDValue HiAddLc = DAG.getNode(ISD::ADDC, dl, MVT::i32, HiAddNc, CarryFromLoAdd);

// Calculate Result as Hi << 32 OR low
	SDValue Lo = DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i64, LoAdd.getValue(0));
	SDValue Hi = DAG.getNode(ISD::ZERO_EXTEND, dl, MVT::i64, HiAddLc.getValue(0));
	
	Hi = DAG.getNode(ISD::SHL, dl, MVT::i64, Hi, DAG.getConstant(32, dl, MVT::i64));

	SDValue Res = DAG.getNode(ISD::OR, dl, MVT::i64, Hi, Lo);
	SDValue CarryOut = HiAddLc.getValue(1);
	SDValue ResCarry[2] = {Res, CarryOut};

	if(Op.getOpcode() == ISD::ADDC) return DAG.getMergeValues(ResCarry, dl);

	return Res;

}


SDValue NaplesPUTargetLowering::LowerOR(SDValue Op, SelectionDAG &DAG) const {
//Se non è 64 bit ritono
	if(Op.getValueType() != MVT::i64) return Op;

	SDLoc dl(Op);

	SDValue op1 = Op.getOperand(0);
	SDValue op2 = Op.getOperand(1);

	SDValue op1Lo = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op1);
	SDValue op1Hi = DAG.getNode(ISD::SRL, dl, MVT::i64, op1, DAG.getConstant(32, dl, MVT::i64));
	op1Hi = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op1Hi);
	
	SDValue op2Lo = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op2);
	SDValue op2Hi = DAG.getNode(ISD::SRL, dl, MVT::i64, op2, DAG.getConstant(32, dl, MVT::i64));
	op2Hi = DAG.getNode(ISD::TRUNCATE, dl, MVT::i32, op2Hi);

	SDValue LoOR = DAG.getNode(ISD::OR, dl, MVT::i32, op1Lo, op2Lo);
 
	SDValue HiOR = DAG.getNode(ISD::OR, dl, MVT::i32, op1Hi, op2Hi);
	
	SDVTList types = DAG.getVTList(MVT::i32, MVT::i32);

	return DAG.getNode(ISD::MERGE_VALUES, dl, types, HiOR, LoOR);  

}

SDValue NaplesPUTargetLowering::LowerADDC(SDValue Op, SelectionDAG &DAG) const {

	SDLoc dl(Op);
	
	return DAG.getNode(ISD::ADD, dl, MVT::i32, Op.getOperand(0), Op.getOperand(1));

}
//Lower the VarArgsFrameIndex, i.e. the FrameIndex for start of the area
//where variable arguments are stored.
SDValue NaplesPUTargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  MachineFunction &MF = DAG.getMachineFunction();
  NaplesPUMachineFunctionInfo *VFI = MF.getInfo<NaplesPUMachineFunctionInfo>();
  SDValue FI = DAG.getFrameIndex(VFI->getVarArgsFrameIndex(),
                                 getPointerTy(DAG.getDataLayout()));
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1),
                      MachinePointerInfo(SV));
}
*/
// Mask off the sign bit: 0 AND MSB
SDValue NaplesPUTargetLowering::LowerFABS(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  MVT ResultVT = Op.getValueType().getSimpleVT();

  MVT IntermediateVT;

  if (ResultVT.isVector()) {
    IntermediateVT = MVT::v16i32;
//            (ResultVT.getVectorElementType() == MVT::f32) ? MVT::v16i32 : MVT::v8i64;
  } else {
    IntermediateVT = MVT::i32;
//            (ResultVT.getScalarType() == MVT::f32) ? MVT::i32 : MVT::i64;
  }

  SDValue rhs = // (IntermediateVT.getScalarType() == MVT::i32)   ? 
                DAG.getConstant(0x7fffffff, DL, MVT::i32);// :
               // DAG.getConstant(0x7fffffffffffffff, DL, MVT::i64);
                
  SDValue iconv;
  if (ResultVT.isVector())
    rhs = DAG.getNode(NaplesPUISD::SPLAT, DL, IntermediateVT, rhs);

  iconv = DAG.getNode(ISD::BITCAST, DL, IntermediateVT, Op.getOperand(0));
  SDValue flipped = DAG.getNode(ISD::AND, DL, IntermediateVT, iconv, rhs);
  return DAG.getNode(ISD::BITCAST, DL, ResultVT, flipped);
}

// isSplatVector - Returns true if N is a BUILD_VECTOR node whose elements are
// all the same.
static bool isSplatVector(SDNode *N) {
  SDValue SplatValue = N->getOperand(0);
  for (unsigned i = 1, e = N->getNumOperands(); i != e; ++i)
    if (N->getOperand(i) != SplatValue)
      return false;

  return true;
}

static bool isZero(SDValue V) {
  ConstantSDNode *C = dyn_cast<ConstantSDNode>(V);
  return C && C->isNullValue();
}

SDValue NaplesPUTargetLowering::LowerBUILD_VECTOR(SDValue Op,
                                               SelectionDAG &DAG) const {
 // MVT VT = Op.getValueType().getSimpleVT();
  SDLoc DL(Op);

  if (isSplatVector(Op.getNode())) {
    // This is a constant node that is duplicated to all lanes.
    // Convert it to a SPLAT node.
    return CheckSplat(Op, Op.getOperand(0), DAG);
  }

  return SDValue(); // Expand
}

// SCALAR_TO_VECTOR loads the scalar register into lane 0 of the register.
// The rest of the lanes are undefined.  For simplicity, we just load the same
// value into all lanes.
SDValue NaplesPUTargetLowering::LowerSCALAR_TO_VECTOR(SDValue Op,
                                                   SelectionDAG &DAG) const {
 // MVT VT = Op.getValueType().getSimpleVT();
  SDLoc DL(Op);
  return CheckSplat(Op, Op.getOperand(0), DAG);
}

bool NaplesPUTargetLowering::isShuffleMaskLegal(ArrayRef<int> Mask,
                                               EVT VT) const {
  if (Mask.size() != 16)
    return false;

  for (int val : Mask) {
    if (val != 0)
      return false;
  }

  return true;
}

//
// Look for patterns that built splats. isShuffleMaskLegal should ensure this
// will only be called with splat masks, but I don't know if there are edge
// cases where it will still be called. Perhaps need to check explicitly (note
// that the shuffle mask doesn't appear to be an operand, but must be accessed
// by casting the SDNode and using a separate accessor).
//TODO: comntrollare non chiaro!
SDValue NaplesPUTargetLowering::LowerVECTOR_SHUFFLE(SDValue Op,
                                                 SelectionDAG &DAG) const {
  // MVT VT = Op.getValueType().getSimpleVT();
  SDLoc DL(Op);

  // Using shufflevector to build a splat like this:
  // %vector = shufflevector <16 x i32> %single, <16 x i32> (don't care),
  //                       <16 x i32> zeroinitializer

  // %single = insertelement <16 x i32> (don't care), i32 %value, i32 0
  if (Op.getOperand(0).getOpcode() == ISD::INSERT_VECTOR_ELT &&
      isZero(Op.getOperand(0).getOperand(2)))
	return CheckSplat(Op, Op.getOperand(0).getOperand(1), DAG);

  // %single = scalar_to_vector i32 %b
  if (Op.getOperand(0).getOpcode() == ISD::SCALAR_TO_VECTOR)
    return CheckSplat(Op, Op.getOperand(0).getOperand(0), DAG);

  return SDValue();
}

// Lower the SELECT Node equals to op(0)? Dst = Op(1) : Dst = Op(2)
SDValue NaplesPUTargetLowering::LowerSELECT(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDLoc DL(Op);
  //EVT Ty = Op.getOperand(0).getValueType();
  // Emit node 'if Op(0) != 0'
  SDValue Pred =DAG.getSetCC(DL, getSetCCResultType(DAG.getDataLayout(),
                              *DAG.getContext(), MVT::i1), Op.getOperand(0),
                              DAG.getConstant(0, DL, MVT::i32), ISD::SETNE);
  // use the SEL_COND_RESULT Node to select the corret input operand
  return DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, Op.getValueType(), Pred,
                     Op.getOperand(1), Op.getOperand(2));
}

// This architecture does not support conditional moves for scalar registers.
// We must convert this into a set of conditional branches. We do this by
// creating a pseudo-instruction SEL_COND_RESULT, which will later be
// transformed.
SDValue NaplesPUTargetLowering::LowerSELECT_CC(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getOperand(0).getValueType();

  // First compare the Op(0) and Op(1) using the condition in Op(4).
  SDValue Pred =
      DAG.getNode(ISD::SETCC, DL, getSetCCResultType(DAG.getDataLayout(),
                                                     *DAG.getContext(), Ty),
                  Op.getOperand(0), Op.getOperand(1), Op.getOperand(4));

  // Then use the comparison result to set the destination equal to
  // Op(2) if true and Op(3) otherwise.
  return DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, Op.getValueType(), Pred,
                     Op.getOperand(2), Op.getOperand(3));
}


// There is no native FNEG instruction, so we emulate it by XORing with
// 0x80000000 (f32) or 0x8000000000000000 (f64).
SDValue NaplesPUTargetLowering::LowerFNEG(SDValue Op, SelectionDAG &DAG) const {
  SDLoc DL(Op);
  MVT ResultVT = Op.getValueType().getSimpleVT();
  MVT IntermediateVT;

  if (ResultVT.isVector()) {
    IntermediateVT = MVT::v16i32;
//            (ResultVT.getVectorElementType() == MVT::f32) ? MVT::v16i32 : MVT::v8i64;
  } else {
    IntermediateVT = MVT::i32;
//            (ResultVT.getScalarType() == MVT::f32) ? MVT::i32 : MVT::i64;
  }

  SDValue rhs = //(IntermediateVT.getScalarType() == MVT::i32)   ? 
                DAG.getConstant(0x80000000, DL, MVT::i32); //:
  //              DAG.getConstant(0x8000000000000000, DL, MVT::i64);
                
  SDValue iconv;
  if (ResultVT.isVector())
    rhs = DAG.getNode(NaplesPUISD::SPLAT, DL, IntermediateVT, rhs);

  iconv = DAG.getNode(ISD::BITCAST, DL, IntermediateVT, Op.getOperand(0));
  SDValue flipped = DAG.getNode(ISD::XOR, DL, IntermediateVT, iconv, rhs);
  return DAG.getNode(ISD::BITCAST, DL, ResultVT, flipped);
}

namespace {

SDValue morphSETCCNode(SDValue Op, NaplesPUISD::NodeType Compare, SelectionDAG &DAG) {
  
  SDLoc DL(Op);
  return DAG.getNode(Compare, DL, Op.getValueType().getSimpleVT(),
                     Op.getOperand(0), Op.getOperand(1));
  }
}

// Convert unordered or don't-care floating point comparisions to ordered
// An ordered comparison checks if neither operand is NaN. Conversely, 
// an unordered comparison checks if either operand is a NaN.
// - Two comparison values are ordered if neither operand is NaN, otherwise they
//   are unordered.
// - An ordered comparison *operation* is always false if either operand is NaN.
//   Unordered is always true if either operand is NaN.
// - The hardware implements ordered comparisons.
// - Clang usually emits ordered comparisons.
SDValue NaplesPUTargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {

  SDLoc DL(Op);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  MVT OperandVT = Op.getOperand(0).getSimpleValueType();
  MVT ResultVT = Op.getSimpleValueType();

  // v16i1 comparisons are a bit odd and not generated by normal C code,
  // so we don't really care how efficient it is. Just delegate to
  // full v16i32 comparisons by sext.
  // (If we did care, we could cook up some bitwise operations.)
  // Note: It has to be sext, not zext, as i1 true is the "all ones" value
  // and hence "negative" for the purpose of signed icmp.
  if (OperandVT == MVT::v16i1) {
    SDValue SExtOp0 = DAG.getNode(ISD::SIGN_EXTEND, DL, MVT::v16i32,
                                  Op.getOperand(0));
    SDValue SExtOp1 = DAG.getNode(ISD::SIGN_EXTEND, DL, MVT::v16i32,
                                  Op.getOperand(1));
    return DAG.getSetCC(DL, MVT::v16i1, SExtOp0, SExtOp1, CC);
  }
  if (ResultVT == MVT::v16i32) {
    SDValue Cond = DAG.getSetCC(DL, MVT::v16i1, Op.getOperand(0), Op.getOperand(1), CC);
    return DAG.getNode(ISD::SIGN_EXTEND_INREG, DL, MVT::v16i32, Cond);
  }
/*  if (ResultVT == MVT::v8i64) {
    SDValue Cond = DAG.getSetCC(DL, MVT::v8i1, Op.getOperand(0), Op.getOperand(1), CC);
    return DAG.getNode(ISD::SIGN_EXTEND_INREG, DL, MVT::v8i64, Cond);
  }
*/
  // The main job of this functions is to convert unordered or don't-care
  // floating point comparisions to ordered ones. But we also map legal
  // comparisons to target-specific SDNodes, to ensure the DAG combiner does
  // not undo the work we do here. This does mean losing out on some
  // optimizations, but this is okay as we're consciously choosing a less
  // optimal/canonical form to legalize these comparisons. Besides,
  // this runs very late in the pipeline, usually a constant or redundant
  // comparison would be folded away long before legalization.
  // Some things to keep in mind:
  // - Two comparison values are ordered if neither operand is NaN, otherwise
  //   they are unordered.
  // - An ordered comparison *operation* is always false if either operand is
  //   NaN. Unordered is always true if either operand is NaN.
  // - The hardware implements ordered comparisons.
  // - Clang usually emits ordered comparisons.
  NaplesPUISD::NodeType ComplementCompare;
  switch (CC) {
  default:
    llvm_unreachable("unhandled float comparison");

  // Ordered comparisons are natively supported, and "don't care"
  // comparisons can be converted to those as well.
  case ISD::SETGT:
  case ISD::SETOGT:
    return morphSETCCNode(Op, NaplesPUISD::FGT, DAG);
  case ISD::SETGE:
  case ISD::SETOGE:
    return morphSETCCNode(Op, NaplesPUISD::FGE, DAG);
  case ISD::SETLT:
  case ISD::SETOLT:
    return morphSETCCNode(Op, NaplesPUISD::FLT, DAG);
  case ISD::SETLE:
  case ISD::SETOLE:
    return morphSETCCNode(Op, NaplesPUISD::FLE, DAG);
  case ISD::SETEQ:
  case ISD::SETOEQ:
    return morphSETCCNode(Op, NaplesPUISD::FEQ, DAG);
  case ISD::SETNE:
  case ISD::SETONE:
    return morphSETCCNode(Op, NaplesPUISD::FNE, DAG);

  // Check for ordered and unordered values by using ordered equality
  // (which will only be false if the values are unordered)
  case ISD::SETO:
  case ISD::SETUO: {
    SDValue Op0 = Op.getOperand(0);
    SDValue IsOrdered =
        DAG.getNode(NaplesPUISD::FEQ, DL, ResultVT, Op0, Op0);
    if (CC == ISD::SETO)
      return IsOrdered;

    // The hardware sets all 16 bits even for scalar comparisons, so negate
    // them all.
    SDValue Negate = DAG.getConstant(0xffff, DL, MVT::i32);
    // if (ResultVT.isVector()) {
    //   Negate = DAG.getNode(NaplesPUISD::MASK_FROM_INT, DL, MVT::v16i1, Negate);
    // }
    return DAG.getNode(ISD::XOR, DL, ResultVT, IsOrdered, Negate);
  }

  // Convert unordered comparisions to ordered by explicitly checking for NaN
  case ISD::SETUEQ:
    ComplementCompare = NaplesPUISD::FNE;
    break;
  case ISD::SETUGT:
    ComplementCompare = NaplesPUISD::FLE;
    break;
  case ISD::SETUGE:
    ComplementCompare = NaplesPUISD::FLT;
    break;
  case ISD::SETULT:
    ComplementCompare = NaplesPUISD::FGE;
    break;
  case ISD::SETULE:
    ComplementCompare = NaplesPUISD::FGT;
    break;
  case ISD::SETUNE:
    ComplementCompare = NaplesPUISD::FEQ;
    break;
  }

  // Take the complementary comparision and invert the result. This will
  // be the same for ordered values, but will always be true for unordered
  // values.
  SDValue Comp2 = morphSETCCNode(Op, ComplementCompare, DAG);
  // The hardware sets all 16 bits even for scalar comparisons, so negate
  // them all.
  SDValue Negate = DAG.getConstant(0xffff, DL, MVT::i32);
  // if (ResultVT.isVector()) {
  //   Negate = DAG.getNode(NaplesPUISD::MASK_FROM_INT, DL, MVT::v16i1, Negate);
  // }
  return DAG.getNode(ISD::XOR, DL, ResultVT, Comp2, Negate);
}

SDValue NaplesPUTargetLowering::LowerCTLZ_ZERO_UNDEF(SDValue Op,
                                                  SelectionDAG &DAG) const {
  SDLoc DL(Op);
  return DAG.getNode(ISD::CTLZ, DL, Op.getValueType(), Op.getOperand(0));
}

SDValue NaplesPUTargetLowering::LowerCTTZ_ZERO_UNDEF(SDValue Op,
                                                  SelectionDAG &DAG) const {
  SDLoc DL(Op);
  return DAG.getNode(ISD::CTTZ, DL, Op.getValueType(), Op.getOperand(0));
}


// The architecture only supports signed integer to floating point. Unsigned 
// integer to floating point is handled using Signed to floating point. If the
// result value is negative (when treated as signed), then add UINT_MAX to the
// resulting floating point value to adjust it.
// This is a simpler version of SelectionDAGLegalize::ExpandLegalINT_TO_FP.
//

SDValue NaplesPUTargetLowering::LowerUINT_TO_FP(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDLoc DL(Op);

  MVT ResultVT = Op.getValueType().getSimpleVT();

  SDValue RVal = Op.getOperand(0);
  MVT RValVT = Op.getOperand(0).getValueType().getSimpleVT();

  SDValue SignedVal = DAG.getNode(ISD::SINT_TO_FP, DL, ResultVT, RVal);

  // Load constant offset to adjust
  SDValue AdjustValue =// (ResultVT.getScalarType() == MVT::f32)       ?
                        DAG.getConstantFP( 4294967296.0, DL, MVT::f32);// : // UINT_MAX (2^32)
  //                      DAG.getConstantFP( 18446744073709551616.0, DL, MVT::f64); // UINT_MAX (2^64)

  
  if (ResultVT.isVector()) {
    // Vector Result
    SDValue ZeroVec;
    SDValue LtIntrinsic;
    SDValue IsNegativeMask;

    if(ResultVT.getScalarType() == MVT::f32) {
      ZeroVec = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v16i32,
                                    DAG.getConstant(0, DL, MVT::i32));
      LtIntrinsic =
          DAG.getConstant(Intrinsic::npu_mask_cmpi32_slt, DL, MVT::i32);
      IsNegativeMask = DAG.getNode(ISD::INTRINSIC_WO_CHAIN, DL, MVT::i32,
                                          LtIntrinsic, RVal, ZeroVec);
    }/* else {
      ZeroVec = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v8i64,
                                    DAG.getConstant(0, DL, MVT::i64));
      LtIntrinsic =
          DAG.getConstant(Intrinsic::npu_mask_cmpi64_slt, DL, MVT::i32);
      IsNegativeMask = DAG.getNode(ISD::INTRINSIC_WO_CHAIN, DL, MVT::i32,
                                          LtIntrinsic, RVal, ZeroVec);
    }
*/
    SDValue AdjustVec =
        DAG.getNode(NaplesPUISD::SPLAT, DL, ResultVT, AdjustValue);
    SDValue Adjusted =
        DAG.getNode(ISD::FADD, DL, ResultVT, SignedVal, AdjustVec);
    // return DAG.getNode(
    //     ISD::INTRINSIC_WO_CHAIN, DL, ResultVT,
    //     DAG.getConstant(Intrinsic::npu_vector_mixf32, DL, MVT::i32),
    //     IsNegativeMask, Adjusted, SignedVal);
    SDValue Ret = DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, ResultVT, IsNegativeMask,
                       Adjusted, SignedVal);
    return Ret;
  } else {
    // Scalar Result.  If the result is negative, add UINT_MASK to make it
    // positive
    SDValue IsNegative =
        DAG.getSetCC(DL, getSetCCResultType(DAG.getDataLayout(),
                                            *DAG.getContext(), RValVT.getScalarType()),
                     RVal, DAG.getConstant(0, DL, RValVT.getScalarType()), ISD::SETLT);
    SDValue Adjusted =
        DAG.getNode(ISD::FADD, DL, ResultVT.getScalarType(), SignedVal, AdjustValue);
    SDValue Ret = DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, ResultVT.getScalarType(), IsNegative,
                       Adjusted, SignedVal);
    return Ret;
  }
}

SDValue NaplesPUTargetLowering::LowerFRAMEADDR(SDValue Op,
                                            SelectionDAG &DAG) const {
  assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) &&
         "Frame address can only be determined for current frame.");

  SDLoc DL(Op);
  MachineFrameInfo &MFI = DAG.getMachineFunction().getFrameInfo();
  MFI.setFrameAddressIsTaken(true);
  EVT VT = Op.getValueType();

  return DAG.getCopyFromReg(DAG.getEntryNode(), DL, NaplesPU::FP_REG, VT);
}

SDValue NaplesPUTargetLowering::LowerRETURNADDR(SDValue Op,
                                             SelectionDAG &DAG) const {
  if (verifyReturnAddressArgumentIsConstant(Op, DAG))
    return SDValue();

  // check the depth
  assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) &&
         "Return address can be determined only for current frame.");

  SDLoc DL(Op);
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MVT VT = Op.getSimpleValueType();
  MFI.setReturnAddressIsTaken(true);

  // Return RA, which contains the return address. Mark it an implicit live-in.
  unsigned Reg = MF.addLiveIn(NaplesPU::RA_REG, getRegClassFor(VT));
  return DAG.getCopyFromReg(DAG.getEntryNode(), DL, Reg, VT);
}

// Return the right intrinsic ID for vector comparison, given the Condition Code.
static Intrinsic::ID intrinsicForVectorCompare(ISD::CondCode CC, bool isFloat) {
  if (isFloat) {
    switch (CC) {
    case ISD::SETOEQ:
    case ISD::SETUEQ:
    case ISD::SETEQ:
      return Intrinsic::npu_mask_cmpf32_eq;

    case ISD::SETONE:
    case ISD::SETUNE:
    case ISD::SETNE:
      return Intrinsic::npu_mask_cmpf32_ne;

    case ISD::SETOGT:
    case ISD::SETUGT:
    case ISD::SETGT:
      return Intrinsic::npu_mask_cmpf32_gt;

    case ISD::SETOGE:
    case ISD::SETUGE:
    case ISD::SETGE:
      return Intrinsic::npu_mask_cmpf32_ge;

    case ISD::SETOLT:
    case ISD::SETULT:
    case ISD::SETLT:
      return Intrinsic::npu_mask_cmpf32_lt;

    case ISD::SETOLE:
    case ISD::SETULE:
    case ISD::SETLE:
      return Intrinsic::npu_mask_cmpf32_le;

    default:; // falls through
    }
  } else {
    switch (CC) {
    case ISD::SETUEQ:
    case ISD::SETEQ:
      return Intrinsic::npu_mask_cmpi32_eq;

    case ISD::SETUNE:
    case ISD::SETNE:
      return Intrinsic::npu_mask_cmpi32_ne;

    case ISD::SETUGT:
      return Intrinsic::npu_mask_cmpi32_ugt;

    case ISD::SETGT:
      return Intrinsic::npu_mask_cmpi32_sgt;

    case ISD::SETUGE:
      return Intrinsic::npu_mask_cmpi32_uge;

    case ISD::SETGE:
      return Intrinsic::npu_mask_cmpi32_sge;

    case ISD::SETULT:
      return Intrinsic::npu_mask_cmpi32_ult;

    case ISD::SETLT:
      return Intrinsic::npu_mask_cmpi32_slt;

    case ISD::SETULE:
      return Intrinsic::npu_mask_cmpi32_ule;

    case ISD::SETLE:
      return Intrinsic::npu_mask_cmpi32_sle;

    default:; // falls through
    }
  }

  llvm_unreachable("unhandled compare code");
}
/*
// Same as intrinsicForVectorCompare but for 64 bit vector types.
static Intrinsic::ID intrinsicForVectorCompare_64(ISD::CondCode CC, bool isFloat) {
  if (isFloat) {
    switch (CC) {
    case ISD::SETOEQ:
    case ISD::SETUEQ:
    case ISD::SETEQ:
      return Intrinsic::npu_mask_cmpf64_eq;

    case ISD::SETONE:
    case ISD::SETUNE:
    case ISD::SETNE:
      return Intrinsic::npu_mask_cmpf64_ne;

    case ISD::SETOGT:
    case ISD::SETUGT:
    case ISD::SETGT:
      return Intrinsic::npu_mask_cmpf64_gt;

    case ISD::SETOGE:
    case ISD::SETUGE:
    case ISD::SETGE:
      return Intrinsic::npu_mask_cmpf64_ge;

    case ISD::SETOLT:
    case ISD::SETULT:
    case ISD::SETLT:
      return Intrinsic::npu_mask_cmpf64_lt;

    case ISD::SETOLE:
    case ISD::SETULE:
    case ISD::SETLE:
      return Intrinsic::npu_mask_cmpf64_le;

    default:; // falls through
    }
  } else {
    switch (CC) {
    case ISD::SETUEQ:
    case ISD::SETEQ:
      return Intrinsic::npu_mask_cmpi64_eq;

    case ISD::SETUNE:
    case ISD::SETNE:
      return Intrinsic::npu_mask_cmpi64_ne;

    case ISD::SETUGT:
      return Intrinsic::npu_mask_cmpi64_ugt;

    case ISD::SETGT:
      return Intrinsic::npu_mask_cmpi64_sgt;

    case ISD::SETUGE:
      return Intrinsic::npu_mask_cmpi64_uge;

    case ISD::SETGE:
      return Intrinsic::npu_mask_cmpi64_sge;

    case ISD::SETULT:
      return Intrinsic::npu_mask_cmpi64_ult;

    case ISD::SETLT:
      return Intrinsic::npu_mask_cmpi64_slt;

    case ISD::SETULE:
      return Intrinsic::npu_mask_cmpi64_ule;

    case ISD::SETLE:
      return Intrinsic::npu_mask_cmpi64_sle;

    default:; // falls through
    }
  }

  llvm_unreachable("unhandled compare code");
}
*/
// This may be used to expand a vector comparison result into a vector.
// Normally, vector compare results are a bitmask, so we need to do a
// predicated transfer to expand it.
// Note that clang seems to assume a vector lane should have 0xffffffff when the
// result is true when folding constants, so we use that value here to be
// consistent, even though that is not what a scalar compare would do.
SDValue NaplesPUTargetLowering::LowerSIGN_EXTEND_INREG(SDValue Op,
                                                    SelectionDAG &DAG) const {
  SDValue SetCcOp = Op.getOperand(0);
  if (SetCcOp.getOpcode() != ISD::SETCC)
    return SDValue();

  SDLoc DL(Op);
  EVT valT = Op.getValueType();

  SDValue Ret;
  if(valT == MVT::v16i32) {
    Intrinsic::ID intrinsic = intrinsicForVectorCompare(
        cast<CondCodeSDNode>(SetCcOp.getOperand(2))->get(),
        SetCcOp.getOperand(0).getValueType().getSimpleVT().isFloatingPoint());
    
    SDValue FalseVal = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v16i32,
                                 DAG.getConstant(0, DL, MVT::i32));
    SDValue TrueVal = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v16i32,
                                DAG.getConstant(-1, DL, MVT::i32));
    SDValue Mask = DAG.getNode(ISD::INTRINSIC_WO_CHAIN, DL, MVT::i32,
                             DAG.getConstant(intrinsic, DL, MVT::i32),
                             SetCcOp.getOperand(0), SetCcOp.getOperand(1));
    Ret = DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, Op.getValueType(), Mask,
                     TrueVal, FalseVal);
  }
/*  if(valT == MVT::v8i64) {
    
    Intrinsic::ID intrinsic = intrinsicForVectorCompare_64(
        cast<CondCodeSDNode>(SetCcOp.getOperand(2))->get(),
        SetCcOp.getOperand(0).getValueType().getSimpleVT().isFloatingPoint());

    SDValue FalseVal = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v8i64,
                                   DAG.getConstant(0, DL, MVT::i64));
    SDValue TrueVal = DAG.getNode(NaplesPUISD::SPLAT, DL, MVT::v8i64,
                                  DAG.getConstant(-1, DL, MVT::i64));
    SDValue Mask = DAG.getNode(ISD::INTRINSIC_WO_CHAIN, DL, MVT::i32,
                               DAG.getConstant(intrinsic, DL, MVT::i32),
                               SetCcOp.getOperand(0), SetCcOp.getOperand(1));
    
    Ret = DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, Op.getValueType(), Mask,
                     TrueVal, FalseVal);
  }*/
  return Ret;
}

SDValue NaplesPUTargetLowering::LowerVSELECT(SDValue Op,
                                            SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getOperand(0).getValueType();
  SDValue SetCcOp = Op.getOperand(0);
  
  Intrinsic::ID intrinsic = intrinsicForVectorCompare(
          ISD::SETNE, false);
    
  SDValue Mask = DAG.getNode(ISD::INTRINSIC_WO_CHAIN, DL, MVT::i32,
                            DAG.getConstant(intrinsic, DL, MVT::i32),
                            SetCcOp, DAG.getConstant(0, DL, Ty.getScalarType()));
  return DAG.getNode(NaplesPUISD::SEL_COND_RESULT, DL, Op.getValueType(), Mask,
                    Op.getOperand(1), Op.getOperand(2));
}


// Hook function used to call the right function to lower Custom-labeled operations.
// Note: The operation must setted as Custum in the class constructor first.
SDValue NaplesPUTargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
 /* case ISD::ADDC:
	if(Op.getValueType() == MVT::i32)
		return LowerADDC(Op, DAG);
	return LowerADD_ADDC(Op, DAG);	
  case ISD::ADD:
	return LowerADD_ADDC(Op, DAG);
  case ISD::OR:
	return LowerOR(Op, DAG);
 */
  case ISD::SDIV:
  case ISD::UDIV:
    if (Op.getValueType().isVector())
        return LowerXDIV_VV(Op, DAG);
    else 
        return LowerXDIV(Op, DAG);
  case ISD::VECTOR_SHUFFLE:
    return LowerVECTOR_SHUFFLE(Op, DAG);
  case ISD::BUILD_VECTOR:
    return LowerBUILD_VECTOR(Op, DAG);
  case ISD::SCALAR_TO_VECTOR:
    return LowerSCALAR_TO_VECTOR(Op, DAG);
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::BlockAddress:
    return LowerBlockAddress(Op, DAG);
  case ISD::ConstantPool:
    return LowerConstantPool(Op, DAG);
  case ISD::SELECT:
    return LowerSELECT(Op, DAG);
  case ISD::VSELECT:
    return LowerVSELECT(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSELECT_CC(Op, DAG);
  case ISD::SETCC:
    return LowerSETCC(Op, DAG);
  case ISD::FNEG:
    return LowerFNEG(Op, DAG);
  case ISD::CTLZ_ZERO_UNDEF:
    return LowerCTLZ_ZERO_UNDEF(Op, DAG);
  case ISD::CTTZ_ZERO_UNDEF:
    return LowerCTTZ_ZERO_UNDEF(Op, DAG);
  case ISD::UINT_TO_FP:
    return LowerUINT_TO_FP(Op, DAG);
  case ISD::FRAMEADDR:
    return LowerFRAMEADDR(Op, DAG);
  case ISD::RETURNADDR:
    return LowerRETURNADDR(Op, DAG);
  case ISD::SIGN_EXTEND_INREG:
    return LowerSIGN_EXTEND_INREG(Op, DAG);
//  case ISD::VASTART:
//    return LowerVASTART(Op, DAG);
  case ISD::FABS:
    return LowerFABS(Op, DAG);
  case ISD::JumpTable:
    return LowerJumpTable(Op, DAG);
  case ISD::INTRINSIC_VOID:
    return LowerINTRINSIC_VOID(Op, DAG);
  default:
    llvm_unreachable("Should not custom lower this!");
  }
}

EVT NaplesPUTargetLowering::getSetCCResultType(const DataLayout &,
                                            LLVMContext &Context,
                                            EVT VT) const {
  if (!VT.isVector())
    return MVT::i32;

  return VT.changeVectorElementTypeToInteger();
}


//===----------------------------------------------------------------------===//
//              NaplesPU Custom Inserted MachineInstructions
//===----------------------------------------------------------------------===//

// This method should be implemented for instructions with the 
// 'usesCustomInserter' flag. Check NaplesPUInstrInfo.td 
MachineBasicBlock *
NaplesPUTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                 MachineBasicBlock *BB) const {
  switch (MI.getOpcode()) {
    // TODO: (Catello) separate vector from scalar
  case NaplesPU::SELECTI:
  case NaplesPU::SELECTF:
//  case NaplesPU::SELECTLI:
//  case NaplesPU::SELECTD:
    return EmitSelectCC(&MI, BB);
  case NaplesPU::SELECTVI:
  case NaplesPU::SELECTVF:
    return EmitSelectCC_Vec32(&MI, BB);
//  case NaplesPU::SELECTVLI:
//  case NaplesPU::SELECTVD:
//    return EmitSelectCC_Vec64(&MI, BB);
  case NaplesPU::LoadI32:
    return EmitLoadI32(&MI, BB);
    /*
  case NaplesPU::LoadI64:
    return EmitLoadI64(&MI, BB);
    */
  case NaplesPU::LoadF32:
    return EmitLoadF32(&MI, BB);
    /*
  case NaplesPU::LoadF64:
    return EmitLoadF64(&MI, BB);
    */
  case NaplesPU::InsertELT32:
    return EmitInsertELT(&MI, BB, NaplesPU::InsertELT32);
//  case NaplesPU::InsertELT64:
//    return EmitInsertELT(&MI, BB, NaplesPU::InsertELT64);
	
// TODO:(Catello) modify atomics in order to suit hardware instructions
  case NaplesPU::ATOMIC_LOAD_ADDR:
    return EmitAtomicBinary(MI, BB, NaplesPU::ADDSSS_32);

  case NaplesPU::ATOMIC_LOAD_ADDI:
    return EmitAtomicBinary(MI, BB, NaplesPU::ADDSSI);

  case NaplesPU::ATOMIC_LOAD_SUBR:
    return EmitAtomicBinary(MI, BB, NaplesPU::SUBSSS_32);

  case NaplesPU::ATOMIC_LOAD_SUBI:
    return EmitAtomicBinary(MI, BB, NaplesPU::SUBSSI);

  case NaplesPU::ATOMIC_LOAD_ANDR:
    return EmitAtomicBinary(MI, BB, NaplesPU::ANDSSS_32);

  case NaplesPU::ATOMIC_LOAD_ANDI:
    return EmitAtomicBinary(MI, BB, NaplesPU::ANDSSI);

  case NaplesPU::ATOMIC_LOAD_ORR:
    return EmitAtomicBinary(MI, BB, NaplesPU::ORSSS_32);

  case NaplesPU::ATOMIC_LOAD_ORI:
    return EmitAtomicBinary(MI, BB, NaplesPU::ORSSI);

  case NaplesPU::ATOMIC_LOAD_XORR:
    return EmitAtomicBinary(MI, BB, NaplesPU::XORSSS_32);

  case NaplesPU::ATOMIC_LOAD_XORI:
    return EmitAtomicBinary(MI, BB, NaplesPU::XORSSI);

  case NaplesPU::ATOMIC_LOAD_NANDR:
    return EmitAtomicBinary(MI, BB, NaplesPU::ANDSSS_32, true);

  case NaplesPU::ATOMIC_LOAD_NANDI:
    return EmitAtomicBinary(MI, BB, NaplesPU::ANDSSI, true);

  case NaplesPU::ATOMIC_SWAP:
    return EmitAtomicBinary(MI, BB, 0);

  case NaplesPU::ATOMIC_CMP_SWAP:
    return EmitAtomicCmpSwap(&MI, BB);

  default:
    llvm_unreachable("unknown atomic operation");
  }
}

// Insert the MOVEIH and MOVEIL instruction couple to load a 32-bit
// integer immediate. 
// Note: Instruction order must be MOVEIH followed by MOVEIL. Because
// MOVEIH sets to 0 the register's 16-lsb.  
void InsertLoad32Immediate (MachineInstr *MI, MachineBasicBlock *BB,
                            const TargetInstrInfo *TII, DebugLoc *DL,
                            unsigned DestReg, int64_t Immediate) {
  // Here is used a workaround to use the same destination virtual register.

	  BuildMI(*BB, *MI, *DL, TII->get(NaplesPU::MOVEIHSI))
	              .addReg(DestReg, RegState::Define)
	              .addImm(((Immediate >> 16) & 0xFFFF));
	  BuildMI(*BB, *MI, *DL, TII->get(NaplesPU::MOVEILSI))
	              .addReg(DestReg)
	              .addImm((Immediate & 0xFFFF));

}

void InsertLoad32ImmediateZeroHigh (MachineInstr *MI, MachineBasicBlock *BB,
                            const TargetInstrInfo *TII, DebugLoc *DL,
                            unsigned DestReg, int64_t Immediate) {

	  BuildMI(*BB, *MI, *DL, TII->get(NaplesPU::MOVEISI))
	              .addReg(DestReg, RegState::Define)
	              .addImm(Immediate);

}
//To insert element in vector TODO: supporta solo v8f32 e i normali
// Insert MI to perform the InsertElement Operation. This is done, creating first
// a mask using the index for the insertion then performing a move (we suppose that
// every operation among vector registers are masked except for load and sotres). 
MachineBasicBlock *
NaplesPUTargetLowering::EmitInsertELT(MachineInstr *MI, MachineBasicBlock *BB,
                          unsigned code) const {

  DebugLoc DL = MI->getDebugLoc();
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();

  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();

  // Creating virtual registers to store temporary values.
  // Note: at this stage the instructions are still in SSA form. So
  //       attention must be paid while creating and using virtual reg.
  unsigned One = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), One);
  unsigned ShiftedVal = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), ShiftedVal);
  unsigned OldMR = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);


  unsigned RetVet = MI->getOperand(0).getReg();
  //InsertLoad32Immediate(MI, BB, TII, &DL, TmpReg1, 1);
  
  // Save the old Mask Register.
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), OldMR)
              .addReg(NaplesPU::MR_REG);
  
  // Create the mask: 1 << index.
  unsigned Immediate = 1;
  // Load 1 into the One VReg.
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(One, RegState::Define)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(One)
              .addImm((Immediate & 0xFFFF));
  // Shift 1 by index (Op(3)).
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::SLLSSS_32), ShiftedVal)
              .addReg(One)
              .addReg(MI->getOperand(3).getReg());
  // Move the result value in the Mask Register.
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(ShiftedVal);

  // Move the scalar value (Op(2)) in the vector destination (Op(0)).
  // The operation is equal to Op(0) <= Move Op(2).
  // Note: MOVE_VS_M is setted as an unmasked instruction like MOVE_VS 
  //       and takes two input operands, the second of which is used to 
  //       tell the code generator that the physical register must be 
  //       the same of the output register.
  if (code == NaplesPU::InsertELT32) {    
    BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VS_M_32),RetVet)
                  .addReg(MI->getOperand(2).getReg(), RegState::Kill)
                  .addReg(MI->getOperand(1).getReg());
  }

/*  if (code == NaplesPU::InsertELT64) {
    BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VS_M_64),RetVet)
                  .addReg(MI->getOperand(2).getReg(), RegState::Kill)
                  .addReg(MI->getOperand(1).getReg());
  }
*/
  // Restore mask register
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(OldMR, RegState::Kill);
  
  // Eliminate the old instruction.
  MI->eraseFromParent();

  return BB;
}

// The EmitLoad functions emit the MIs necessary to the loading of
// a 32-bit immediate value.
MachineBasicBlock *
NaplesPUTargetLowering::EmitLoadI32(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {

  DebugLoc DL = MI->getDebugLoc();
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  
  // Get the integer immediate value.
  int64_t ImmOp = MI->getOperand(1).getImm();
    
if((ImmOp & 0xFFFF0000) != 0)
  InsertLoad32Immediate(MI, BB, TII, &DL, MI->getOperand(0).getReg(), ImmOp);
else
  InsertLoad32ImmediateZeroHigh(MI, BB, TII, &DL, MI->getOperand(0).getReg(), ImmOp);

  MI->eraseFromParent();

  return BB;
}

MachineBasicBlock *
NaplesPUTargetLowering::EmitLoadF32(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {

  DebugLoc DL = MI->getDebugLoc();
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  
  // Get the float imeediate value.
  APFloat ImmOp = MI->getOperand(1).getFPImm()->getValueAPF();
  // Call the fuction that generates the MIs, passing it the immediate value
  // bitcasted to integer.
  InsertLoad32Immediate(MI, BB, TII, &DL, MI->getOperand(0).getReg(), ImmOp.bitcastToAPInt().getLoBits(32).getZExtValue());

  MI->eraseFromParent();

  return BB;
}


/*
MachineBasicBlock *
NaplesPUTargetLowering::EmitLoadI64(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {

  DebugLoc DL = MI->getDebugLoc();
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();

  // Create the destination register
  unsigned DstReg = MI->getOperand(0).getReg();//MRI.createVirtualRegister(&NaplesPU::GPR64RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), DstReg);

  int64_t ImmOp = MI->getOperand(1).getImm();

  unsigned Immediate = ((ImmOp >> 32) & 0xffffffff);

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(DstReg, RegState::Define, NaplesPU::sub_odd)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(DstReg, 0, NaplesPU::sub_odd)
              .addImm((Immediate & 0xFFFF));

  Immediate = (ImmOp & 0xffffffff);

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(DstReg, 0, NaplesPU::sub_even)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(DstReg, 0, NaplesPU::sub_even)
              .addImm((Immediate & 0xFFFF));

// MRI.replaceRegWith(MI->getOperand(0).getReg(), DstReg);

  MI->eraseFromParent();

  return BB;
}

MachineBasicBlock *
NaplesPUTargetLowering::EmitLoadF64(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {

  DebugLoc DL = MI->getDebugLoc();
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();

  // Create the destination register
  unsigned DstReg = MI->getOperand(0).getReg();
  // MRI.createVirtualRegister(&NaplesPU::GPR64RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), DstReg);

  APFloat ImmOp = MI->getOperand(1).getFPImm()->getValueAPF();

  uint64_t Immediate = ImmOp.bitcastToAPInt().getHiBits(32).getZExtValue();

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(DstReg, RegState::Define, NaplesPU::sub_odd)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(DstReg, 0, NaplesPU::sub_odd)
              .addImm((Immediate & 0xFFFF));

  Immediate = ImmOp.bitcastToAPInt().getLoBits(32).getZExtValue();

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(DstReg, 0, NaplesPU::sub_even)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(DstReg, 0, NaplesPU::sub_even)
              .addImm((Immediate & 0xFFFF));
*/
 // MRI.replaceRegWith(MI->getOperand(0).getReg(), DstReg);
/*
  unsigned TmpReg1 = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  unsigned TmpReg2 = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  //unsigned TmpReg3 = MRI.createVirtualRegister(&NaplesPU::GPR64RegClass);

  APFloat ImmOp = MI->getOperand(1).getFPImm()->getValueAPF();

  InsertLoad32Immediate(MI, BB, TII, &DL, TmpReg1, ImmOp.bitcastToAPInt().getHiBits(32).getZExtValue());
  InsertLoad32Immediate(MI, BB, TII, &DL, TmpReg2, ImmOp.bitcastToAPInt().getLoBits(32).getZExtValue());
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::INSERT_SUBREG), MI->getOperand(0).getReg())
      .addReg(MI->getOperand(0).getReg())
      .addReg(TmpReg1)
      .addImm(NaplesPU::sub_odd);
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::INSERT_SUBREG), MI->getOperand(0).getReg())
      .addReg(MI->getOperand(0).getReg())
      .addReg(TmpReg2)
      .addImm(NaplesPU::sub_even);

*/
/*
  MI->eraseFromParent();

  return BB;
}
*/

// Generates the MIs to lower the SelectCC instruction. It is lowered generating
// the equivalent of:
//        (pred != 0) ? dest = trueval : dest = falseval;
// This is done using the diamond control-flow pattern.
// We create two Basic Blocks and a phi instruction generating:
//                [ThisMBB]
//                BNEZ pred
//                    |
//           (true)---+---(false)
//             |             |
//             |             \/
//             |         [CopyMBB]
//             |             |
//             +->[SinkMBB]<-+        
// PHI dst, trueval, ThisMBB, falseval, CopyMBB   
MachineBasicBlock *
NaplesPUTargetLowering::EmitSelectCC(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();

  // The instruction we are replacing is SELECTI (Dest, predicate, trueval,
  // falseval)

  // To "insert" a SELECT_CC instruction, we actually have to rewrite it into a
  // diamond control-flow pattern.  The incoming instruction knows the
  // destination vreg to set, the condition code register to branch on, the
  // true/false values to select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator It = BB->getIterator();
  ++It;

  //  ThisMBB:
  //  ...
  //   TrueVal = ...
  //   setcc r1, r2, r3
  //   if r1 goto copy1MBB
  //   fallthrough --> Copy0MBB
  MachineBasicBlock *ThisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *Copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *SinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(It, Copy0MBB);
  F->insert(It, SinkMBB);

  // Transfer the remainder of BB and its successor edges to SinkMBB.
  SinkMBB->splice(SinkMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  SinkMBB->transferSuccessorsAndUpdatePHIs(BB);

  // Next, add the true and fallthrough blocks as its successors.
  BB->addSuccessor(Copy0MBB);
  BB->addSuccessor(SinkMBB);

  BuildMI(BB, DL, TII->get(NaplesPU::BNEZ))
      .addReg(MI->getOperand(1).getReg())
      .addMBB(SinkMBB);
      
  //  Copy0MBB:
  //   %FalseValue = ...
  //   # fallthrough to SinkMBB
  BB = Copy0MBB;

  // Update machine-CFG edges
  BB->addSuccessor(SinkMBB);

  //  SinkMBB:
  //   %Result = phi [ %TrueValue, ThisMBB ], [ %FalseValue, Copy0MBB ]
  //  ...
  BB = SinkMBB;

  BuildMI(*BB, BB->begin(), DL, TII->get(NaplesPU::PHI),
          MI->getOperand(0).getReg())
      .addReg(MI->getOperand(2).getReg())
      .addMBB(ThisMBB)
      .addReg(MI->getOperand(3).getReg())
      .addMBB(Copy0MBB);

  MI->eraseFromParent(); // The pseudo instruction is gone now.

  return BB;
}

// This is the 32-bit vector variant of the EmitSelectCC.
// We use the mask register to perform the right copy inthe dstination vector.
MachineBasicBlock *
NaplesPUTargetLowering::EmitSelectCC_Vec32(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();
  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();
  // To emit the SEL_COND_RESULT, we first save the mask register
  unsigned OldMR = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32))
              .addReg(OldMR, RegState::Define)
              .addReg(NaplesPU::MR_REG);

  // Then copy the condition to the mask register
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(MI->getOperand(1).getReg());
           
  // Perform then action
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VV_32), MI->getOperand(0).getReg())
              .addReg(MI->getOperand(2).getReg());

  // Invert  the mask register
  unsigned NegOne = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), NegOne);

  unsigned Immediate = 0xFFFFFFFF;

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(NegOne, RegState::Define)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(NegOne)
              .addImm((Immediate & 0xFFFF));

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::XORSSS_32), NaplesPU::MR_REG)
              .addReg(NaplesPU::MR_REG)
              .addReg(NegOne, RegState::Kill);

  // Perform else action
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VV_32))
              .addReg(MI->getOperand(0).getReg())
              .addReg(MI->getOperand(3).getReg());

  // Restore mask register
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(OldMR, RegState::Kill);
  
  MI->eraseFromParent(); // The pseudo instruction is gone now.
  
  return BB;
}

/*
MachineBasicBlock *
NaplesPUTargetLowering::EmitSelectCC_Vec64(MachineInstr *MI,
                                  MachineBasicBlock *BB) const {
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();
  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();

  // To emit the SEL_COND_RESULT, we first save the mask register
  unsigned OldMR = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32))
              .addReg(OldMR, RegState::Define)
              .addReg(NaplesPU::MR_REG);

  // Then copy the condition to the mask register
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(MI->getOperand(1).getReg());
           
  // Perform then action
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VV_64), MI->getOperand(0).getReg())
              .addReg(MI->getOperand(2).getReg());

  // Invert  the mask register
  unsigned NegOne = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  //BuildMI(*BB, MI, DL, TII->get(NaplesPU::IMPLICIT_DEF), NegOne);

  unsigned Immediate = 0xFFFFFFFF;

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEIHSI))
              .addReg(NegOne, RegState::Define)
              .addImm(((Immediate >> 16) & 0xFFFF));
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVEILSI))
              .addReg(NegOne)
              .addImm((Immediate & 0xFFFF));

  BuildMI(*BB, MI, DL, TII->get(NaplesPU::XORSSS_32), NaplesPU::MR_REG)
              .addReg(NaplesPU::MR_REG)
              .addReg(NegOne, RegState::Kill);

  // Perform else action
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_VV_64))
              .addReg(MI->getOperand(0).getReg())
              .addReg(MI->getOperand(3).getReg());

  // Restore mask register
  BuildMI(*BB, MI, DL, TII->get(NaplesPU::MOVE_SS_32), NaplesPU::MR_REG)
              .addReg(OldMR, RegState::Kill);

  MI->eraseFromParent(); // The pseudo instruction is gone now.
  
  return BB;
}
*/
// TODO: (Catello) lack of hardware atomic instructions

MachineBasicBlock *
NaplesPUTargetLowering::EmitAtomicBinary(MachineInstr &MI, MachineBasicBlock *BB,
                                      unsigned Opcode, bool InvertResult) const {
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();

  unsigned Dest = MI.getOperand(0).getReg();
  unsigned Ptr = MI.getOperand(1).getReg();
  DebugLoc DL = MI.getDebugLoc();
  MachineRegisterInfo &MRI = BB->getParent()->getRegInfo();
  unsigned OldValue = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
  unsigned Success = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);

  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction *MF = BB->getParent();
  MachineFunction::iterator It = BB->getIterator();
  ++It;

  MachineBasicBlock *LoopMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *ExitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MF->insert(It, LoopMBB);
  MF->insert(It, ExitMBB);

  // Transfer the remainder of BB and its successor edges to ExitMBB.
  ExitMBB->splice(ExitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  ExitMBB->transferSuccessorsAndUpdatePHIs(BB);

  //  ThisMBB:
  BB->addSuccessor(LoopMBB);

  //  LoopMBB:
  BB = LoopMBB;
  //BuildMI(BB, DL, TII->get(NaplesPU::LOAD_SYNC), OldValue).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(NaplesPU::L32W_Mainmem), OldValue).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(NaplesPU::MOVE_SS_32), Dest).addReg(OldValue);

  unsigned NewValue;
  if (Opcode != 0) {
    // Perform an operation
    NewValue = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
    if (MI.getOperand(2).getType() == MachineOperand::MO_Register) {
      BuildMI(BB, DL, TII->get(Opcode), NewValue)
          .addReg(OldValue)
          .addReg(MI.getOperand(2).getReg());
    } else if (MI.getOperand(2).getType() == MachineOperand::MO_Immediate) {
      BuildMI(BB, DL, TII->get(Opcode), NewValue)
          .addReg(OldValue)
          .addImm(MI.getOperand(2).getImm());
    } else
      llvm_unreachable("Unknown operand type");

    if (InvertResult) {
      // This is used with AND to create NAND.
      unsigned int Inverted = MRI.createVirtualRegister(&NaplesPU::GPR32RegClass);
      BuildMI(BB, DL, TII->get(NaplesPU::XORSSI), Inverted)
          .addReg(NewValue)
          .addImm(-1);
      NewValue = Inverted;
    }
  } else
    NewValue = OldValue; // This is just swap: use old value

  //BuildMI(BB, DL, TII->get(NaplesPU::STORE_SYNC), Success)
  BuildMI(BB, DL, TII->get(NaplesPU::S32W_Mainmem))
      .addReg(NewValue)
      .addReg(Ptr)
      .addImm(0);
  //BuildMI(BB, DL, TII->get(NaplesPU::BEQZ)).addReg(Success).addMBB(LoopMBB);
  BuildMI(BB, DL, TII->get(NaplesPU::BEQZ)).addImm(1).addMBB(LoopMBB);
  BB->addSuccessor(LoopMBB);
  BB->addSuccessor(ExitMBB);

  //  ExitMBB:
  BB = ExitMBB;
  MI.eraseFromParent(); // The instruction is gone now.
  return BB;
}

MachineBasicBlock *
NaplesPUTargetLowering::EmitAtomicCmpSwap(MachineInstr *MI,
                                       MachineBasicBlock *BB) const {
  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::i32);
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();

  unsigned Dest = MI->getOperand(0).getReg();
  unsigned Ptr = MI->getOperand(1).getReg();
  unsigned OldVal = MI->getOperand(2).getReg();
  unsigned NewVal = MI->getOperand(3).getReg();

  unsigned Success = RegInfo.createVirtualRegister(RC);
  unsigned CmpResult = RegInfo.createVirtualRegister(RC);

  // insert new blocks after the current block
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineBasicBlock *Loop1MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *Loop2MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *ExitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineFunction::iterator It = BB->getIterator();
  ++It;
  MF->insert(It, Loop1MBB);
  MF->insert(It, Loop2MBB);
  MF->insert(It, ExitMBB);

  // Transfer the remainder of BB and its successor edges to ExitMBB.
  ExitMBB->splice(ExitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  ExitMBB->transferSuccessorsAndUpdatePHIs(BB);

  //  ThisMBB:
  //    ...
  //    fallthrough --> Loop1MBB
  BB->addSuccessor(Loop1MBB);
  Loop1MBB->addSuccessor(ExitMBB);
  Loop1MBB->addSuccessor(Loop2MBB);
  Loop2MBB->addSuccessor(Loop1MBB);
  Loop2MBB->addSuccessor(ExitMBB);

  // Loop1MBB:
  //   load.sync Dest, 0(Ptr)
  //   setne cmpresult, Dest, oldval
  //   btrue cmpresult, ExitMBB
  BB = Loop1MBB;
  //BuildMI(BB, DL, TII->get(NaplesPU::LOAD_SYNC), Dest).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(NaplesPU::L32W_Mainmem), Dest).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(NaplesPU::SNESISSS_32), CmpResult)
      .addReg(Dest)
      .addReg(OldVal);
  BuildMI(BB, DL, TII->get(NaplesPU::BNEZ)).addReg(CmpResult).addMBB(ExitMBB);

  // Loop2MBB:
  //   move success, newval			; need a temporary because
  //   store.sync success, 0(Ptr)	; store.sync will clobber success
  //   bfalse success, Loop1MBB
  BB = Loop2MBB;
  //BuildMI(BB, DL, TII->get(NaplesPU::STORE_SYNC), Success)
  BuildMI(BB, DL, TII->get(NaplesPU::S32W_Mainmem))
      .addReg(NewVal)
      .addReg(Ptr)
      .addImm(0);
  BuildMI(BB, DL, TII->get(NaplesPU::BEQZ)).addImm(1).addMBB(Loop1MBB);

  MI->eraseFromParent(); // The instruction is gone now.

  return ExitMBB;
}

//===----------------------------------------------------------------------===//
//                         NaplesPU Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the Type of
/// constraint it is for this target.
NaplesPUTargetLowering::ConstraintType
NaplesPUTargetLowering::getConstraintType(StringRef Constraint) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 's':
    case 'r':
    case 'v':
      return C_RegisterClass; //Constraint represents any of register(s) in class.
    default:
      break;
    }
  }

  return TargetLowering::getConstraintType(Constraint);
}
 

std::pair<unsigned, const TargetRegisterClass *>
NaplesPUTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                  StringRef Constraint,
                                                  MVT VT) const {
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 's':
    case 'r':
      return std::make_pair(0U, &NaplesPU::GPR32RegClass);
    case 'v':
  /*    if (VT == MVT::v8i64 || VT == MVT::v8f64)
        return std::make_pair(0U, &NaplesPU::VR512LRegClass);
      else*/
        return std::make_pair(0U, &NaplesPU::VR512WRegClass);
    }
  }

  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

//TODO: ci sono altri metodi che potrebbero essere implementati

bool NaplesPUTargetLowering::isOffsetFoldingLegal(
    const GlobalAddressSDNode *GA) const {
  // The NaplesPU target isn't yet aware of offsets.
  return false;
}

//TODO: Return true when implemented the integer divider
bool NaplesPUTargetLowering::isIntDivCheap(EVT, AttributeList) const {
  return false;
}


bool NaplesPUTargetLowering::shouldInsertFencesForAtomic(const Instruction *I) const {
  return true;
}
