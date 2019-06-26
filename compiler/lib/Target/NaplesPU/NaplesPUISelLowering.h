//===-- NaplesPUISelLowering.h - NaplesPU DAG Lowering Interface -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that NaplesPU uses to lower LLVM code into
// a selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_NAPLESPU_NAPLESPUISELLOWERING_H
#define LLVM_LIB_TARGET_NAPLESPU_NAPLESPUISELLOWERING_H

#include "NaplesPU.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class NaplesPUSubtarget;

namespace NaplesPUISD {
  enum NodeType{
    FIRST_NUMBER = ISD::BUILTIN_OP_END,
    CALL,     // A call instruction.
    RET_FLAG, // Return with a flag operand.
    SPLAT,    // Copy scalar register into all lanes of a vector
    SEL_COND_RESULT,
    LEAH,   // Moveih node
    LEAL,   // Moveil node

    FGT,
    FGE,
    FLT,
    FLE,
    FEQ,
    FNE,
  };
}

class NaplesPUTargetLowering : public TargetLowering {
public:
  static const NaplesPUTargetLowering *create(const NaplesPUTargetMachine &TM,
                                           const NaplesPUSubtarget &STI);

  explicit NaplesPUTargetLowering(const TargetMachine &TM,
                               const NaplesPUSubtarget &STI);
/// This callback is invoked for operations that are unsupported by the
/// target, which are registered to use 'custom' lowering, and whose defined
/// values are all legal.  If the target has no operations that require custom
/// lowering, it need not implement this.  The default implementation of this
/// aborts.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

/// This method should be implemented by targets that mark instructions with
/// the 'usesCustomInserter' flag.
  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *MBB) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;
/// Given a constraint, return the type of constraint it is for this target.
  ConstraintType getConstraintType(StringRef Constraint) const override;
/// Given a physical register constraint (e.g.  {edx}), return the register
/// number and the register class for the register.
///
/// Given a register class constraint, like 'r', if this corresponds directly
/// to an LLVM register class, return a register of 0 and the register class
/// pointer.
///
/// This should only be used for C_Register constraints.  On error, this
/// returns a register number of 0 and a null register class pointer.
  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;
/// Return true if folding a constant offset with the given GlobalAddress is
/// legal.  It is frequently not legal in PIC relocation models.
  bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;

  EVT getSetCCResultType(const DataLayout &, LLVMContext &Context,
                         EVT VT) const override;
/// This hook must be implemented to lower outgoing return values, described
/// by the Outs array, into the specified DAG. The implementation should
/// return the resulting token chain value.
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc&,
                      SelectionDAG &DAG) const override;
/// This hook must be implemented to lower the incoming (formal) arguments,
/// described by the Ins array, into the specified DAG. The implementation
/// should fill in the InVals array with legal-type argument values, and
/// return the resulting token chain value.
///
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins, 
                               const SDLoc&, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;
/// This hook must be implemented to lower calls into the specified
/// DAG. The outgoing arguments to the call are described by the Outs array,
/// and the values to be returned by the call are described by the Ins
/// array. The implementation should fill in the InVals array with legal-type
/// return values from the call, and return the resulting token chain value.
  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

/// Targets can use this to indicate that they only support *some*
/// VECTOR_SHUFFLE operations, those with specific masks.  By default, if a
/// target supports the VECTOR_SHUFFLE node, all mask values are assumed to be
/// legal.
bool isShuffleMaskLegal(ArrayRef<int> Mask, EVT VT) const override;
/// Return true if integer divide is usually cheaper than a sequence of
/// several shifts, adds, and multiplies for this target.
/// The definition of "cheaper" may depend on whether we're optimizing
/// for speed or for size.
  bool isIntDivCheap(EVT VT, AttributeList Attr) const override;
/// Whether AtomicExpandPass should automatically insert fences and reduce
/// ordering for this atomic. This should be true for most architectures with
/// weak memory ordering. Defaults to false.
  bool shouldInsertFencesForAtomic(const Instruction *I) const override;

private:
  MachineBasicBlock *EmitInsertELT(MachineInstr *MI, MachineBasicBlock *BB, 
                                      unsigned code) const;
  MachineBasicBlock *EmitLoadI32(MachineInstr *MI,
                                    MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitLoadI64(MachineInstr *MI,
                                    MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitLoadF32(MachineInstr *MI,
                                    MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitLoadF64(MachineInstr *MI,
                                    MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitSelectCC(MachineInstr *MI,
                                  MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitSelectCC_Vec32(MachineInstr *MI,
                                  MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitSelectCC_Vec64(MachineInstr *MI,
                                  MachineBasicBlock *BB) const;
  MachineBasicBlock *EmitAtomicBinary(MachineInstr &MI, MachineBasicBlock *BB,
                                      unsigned Opcode, bool InvertResult=false) const;
  MachineBasicBlock *EmitAtomicCmpSwap(MachineInstr *MI,
                                       MachineBasicBlock *BB) const;
  template <class NodeTy>
  SDValue getAddr(const NodeTy *N, SelectionDAG &DAG) const;
  SDValue getTargetNode(const BlockAddressSDNode *N, EVT Ty,
                                           SelectionDAG &DAG) const;
  SDValue getTargetNode(const ConstantPoolSDNode *N, EVT Ty,
                                           SelectionDAG &DAG) const;
  SDValue getTargetNode(const GlobalAddressSDNode *N, EVT Ty,
                                           SelectionDAG &DAG) const;
  SDValue getTargetNode(const JumpTableSDNode *N, EVT Ty,
                                           SelectionDAG &DAG) const;
  SDValue LowerBUILD_VECTOR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerJumpTable(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerVECTOR_SHUFFLE(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerINSERT_VECTOR_ELT(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSELECT(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerVSELECT(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerConstantPool(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFP_EXTEND(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFDIV(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerXDIV(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerXDIV_VV(SDValue Op, SelectionDAG &DAG) const;
  //SDValue LowerBR_JT(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBRIND(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSCALAR_TO_VECTOR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFNEG(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerEXTRACT_VECTOR_ELT(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSETCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerCTLZ_ZERO_UNDEF(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerCTTZ_ZERO_UNDEF(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerUINT_TO_FP(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFTRUNC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerRETURNADDR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSIGN_EXTEND_INREG(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;
  //SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerFABS(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerINTRINSIC_VOID(SDValue Op, SelectionDAG &DAG) const;

//FRANCESCO
/*
  SDValue LowerADD_ADDC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerADDC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerOR(SDValue Op, SelectionDAG &DAG) const;
*/
  const NaplesPUSubtarget &Subtarget;
};
} // end namespace llvm

#endif // NAPLESPU_ISELLOWERING_H
