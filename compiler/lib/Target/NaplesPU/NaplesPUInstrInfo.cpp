//===-- NaplesPUInstrInfo.cpp - NaplesPU Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the NaplesPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "NaplesPU.h"
#include "NaplesPUInstrInfo.h"
#include "NaplesPUMachineFunctionInfo.h"
#include "NaplesPURegisterInfo.h"
#include "NaplesPUSubtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "NaplesPUGenInstrInfo.inc"

const NaplesPUInstrInfo *NaplesPUInstrInfo::create(NaplesPUSubtarget &ST) {
  return new NaplesPUInstrInfo(ST);
}
NaplesPUInstrInfo::NaplesPUInstrInfo(NaplesPUSubtarget &ST)
    : NaplesPUGenInstrInfo(NaplesPU::ADJCALLSTACKDOWN, NaplesPU::ADJCALLSTACKUP), RI() {}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned NaplesPUInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  if (MI.getOpcode() == NaplesPU::L32W_Mainmem ||
//      MI.getOpcode() == NaplesPU::L64L_Mainmem ||
      MI.getOpcode() == NaplesPU::LV32WMainmem_U// ||
//      MI.getOpcode() == NaplesPU::LV64LMainmem_U
      ) {
    if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
        MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
  }
  return 0;
}

/// isStoreToStackSlot - If the specified machine instruction is a direct
/// store to a stack slot, return the virtual or physical register number of
/// the source reg along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than storing to the stack slot.
unsigned NaplesPUInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                            int &FrameIndex) const {
  if (MI.getOpcode() == NaplesPU::S32W_Mainmem ||
  //    MI.getOpcode() == NaplesPU::S64L_Mainmem ||
      MI.getOpcode() == NaplesPU::SV32WMainmem_U// ||
  //    MI.getOpcode() == NaplesPU::SV64LMainmem_U
      ) {
    if (MI.getOperand(0).isFI() && MI.getOperand(1).isImm() &&
        MI.getOperand(1).getImm() == 0) {
      FrameIndex = MI.getOperand(0).getIndex();
      return MI.getOperand(2).getReg();
    }
  }
  return 0;
}

//TODO: controllare le funzioni di supporto per l'analyze branch
// vedere come mai JMPSR_OFF e JMPSR_REG non sono considerate
static bool isUncondBranchOpcode(int Opc) { return Opc == NaplesPU::JMP; }

static bool isCondBranchOpcode(int Opc) {
  return Opc == NaplesPU::BEQZ || Opc == NaplesPU::BNEZ;
}


/*
  analyzeBranch analizza la fine del MachineBasicBlock (che contiene una sequenza di istruzioni)
  per verificare l'esistenza di branch.
  Ha come input il blocco MBB e come output il blocco TBB (TrueBasicBlock) come
  blocco di destinazione se la condizione del branch è true, e il blocco FBB
  (FalseBasicBlock) come blocco di destinazione se la condizione del branch è false.
  Restituisce il valore booleano false quando:
  1) Se il blocco finisce senza branches, lasciando TBB e FBB a null;
  2) Se il blocco termina con un branch incondizionale, setta TBB pari al
     blocco di destinazione;
  3) Se il blocco termina con un branch condizionale che salta al blocco successore
     setta TBB al blocco destinazione e una lista di operandi che valutano la condizione.
     Tali operandi possono essere passati ad altri metodi di TargetInstrInfo per creare
     nuovi branches;
  4) Se il blocco termina con un branch condizionale seguito da un branch incondizionato
     restituisce la destinazione 'true' in TBB, la destinazione 'false' in FBB,
     e una lista di operandi usata per valutare la condizione. Tali operandi possono essere passati ad altri metodi di TargetInstrInfo per creare
     nuovi branches.

  Se AllowModify è vero, tale metodo può modificare il basicblock

  NOTA: Tale funzione è richiamata per ottimizzazione durante il branchfolding
        e l'IfConversion
*/
bool NaplesPUInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                   MachineBasicBlock *&TBB,
                                   MachineBasicBlock *&FBB,
                                   SmallVectorImpl<MachineOperand> &Cond,
                                   bool AllowModify) const {
								  
	TBB = nullptr;
	FBB = nullptr;								  
	MachineBasicBlock::iterator I = MBB.end();
	if (I == MBB.begin())
	return false; // Empty blocks are easy.
	--I;

	// Walk backwards from the end of the basic block until the branch is
	// analyzed or we give up.
	while (I->isTerminator() || I->isDebugValue()) {
	// Flag to be raised on unanalyzeable instructions. This is useful in cases
	// where we want to clean up on the end of the basic block before we bail
	// out.
	bool CantAnalyze = false;

	// Skip over DEBUG values
	while (I->isDebugValue()) {
	  if (I == MBB.begin())
		return false;
	  --I;
	}

	if (isUncondBranchOpcode(I->getOpcode())) {
	  TBB = I->getOperand(0).getMBB();
	} else if (isCondBranchOpcode(I->getOpcode())) {
	  // Bail out if we encounter multiple conditional branches.
	  if (!Cond.empty())
		return true;

	  assert(!FBB && "FBB should have been null.");
	  // The goto at the end is only the false fall through.
	  FBB = TBB;
	  TBB = I->getOperand(1).getMBB();

	  Cond.push_back(MachineOperand::CreateImm(I->getOpcode()));
	  Cond.push_back(I->getOperand(0));
	} else if (I->isReturn()) {
	  // Returns can't be analyzed, but we should run cleanup.
	  CantAnalyze = true;
	} else {
	  // We encountered other unrecognized terminator. Bail out immediately.
	  return true;
	}

	// Cleanup code - to be run for unconditional branches and
	//                returns.
	if (isUncondBranchOpcode(I->getOpcode()) ||
		   I->isReturn()) {
	  // Forget any previous condition branch information - it no longer applies.
	  Cond.clear();
	  FBB = nullptr;

	  // If we can modify the function, delete everything below this
	  // unconditional branch.
	  if (AllowModify) {
		MachineBasicBlock::iterator DI = std::next(I);
		while (DI != MBB.end()) {
		  MachineInstr &InstToDelete = *DI;
		  ++DI;
		  InstToDelete.eraseFromParent();
		}
	  }
	}

	if (CantAnalyze)
	  return true;

	if (I == MBB.begin())
	  return false;

	--I;
	}

	// We made it past the terminators without bailing out - we must have
	// analyzed this branch successfully.
	return false;

}

/*
  Inserisce codice di salto alla fine del MachineBasicBlock specifica.
  I parametri di ingresso e uscita sono gli stesssi del meetofo AnalyzeBranch.
  Tale metodo è invocato solo quando l'AnalyzeBranch ha esito positivo (return false).
  Restituisce il numero di istruzioni inserite.

  Può essere invocata anche dal tail merging (ottimizzazione) per aggiungere salti
  incondizionali dove l'AnalyzeBranch non può essere applicata. Questa funzione deve
  essere implementata, altrimenti si deve disabilitare il tail merging.
*/
unsigned
NaplesPUInstrInfo::insertBranch(MachineBasicBlock &MBB,
                                       MachineBasicBlock *TBB, // If true
                                       MachineBasicBlock *FBB, // If false
                                       ArrayRef<MachineOperand> Cond,
                                       const DebugLoc &DL,
                                       int *BytesAdded) const{ 
  
  assert(TBB);
  if (FBB) {
    // Has a false block, this is a two way conditional branch
    BuildMI(&MBB, DL, get(Cond[0].getImm())).add(Cond[1]).addMBB(TBB);
    BuildMI(&MBB, DL, get(NaplesPU::JMP)).addMBB(FBB);
    return 2;
  }

  if (Cond.empty()) {
    // Unconditional branch
    BuildMI(&MBB, DL, get(NaplesPU::JMP)).addMBB(TBB);
    return 1;
  }

  // One-way conditional branch
  BuildMI(&MBB, DL, get(Cond[0].getImm())).add(Cond[1]).addMBB(TBB);
  return 1;
}

/*
  Rimuove il codice di branching alla fine dello specificato MBB. Questo metodo
  è invocato quando l'AnalyzeBranch restituisce successo (return false). restituisce
  il numero di istruzioni eliminate.
*/
unsigned NaplesPUInstrInfo::removeBranch(MachineBasicBlock &MBB,
                                          int *BytesRemoved) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin()) {
    --I;

    if (I->isDebugValue())
      continue;

    if (!isUncondBranchOpcode(I->getOpcode())
      && !isCondBranchOpcode(I->getOpcode()))
      break; // Not a branch

    I->eraseFromParent();
    I = MBB.end();
    ++Count;
  }
  return Count;
}


/*
  Emette istruzioni per la copia tra due registri fisici.
  Deve supportare copie tra qualsiasi classe di registri legali, così come
  tra qualsiasi copia cross-class durante la instruction selection.
*/
void NaplesPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I, 
                                 const DebugLoc &DL, unsigned DestReg, 
                                 unsigned SrcReg, bool KillSrc) const {
  bool destIsScalar32 = NaplesPU::GPR32RegClass.contains(DestReg);
  //bool destIsScalar64 = NaplesPU::GPR64RegClass.contains(DestReg);
  bool destIsVector16x32 = NaplesPU::VR512WRegClass.contains(DestReg);
 // bool destIsVector8x64 = NaplesPU::VR512LRegClass.contains(DestReg);
  bool srcIsScalar32 = NaplesPU::GPR32RegClass.contains(SrcReg);
  //bool srcIsScalar64 = NaplesPU::GPR64RegClass.contains(SrcReg);
  bool srcIsScalar16x32 = NaplesPU::VR512WRegClass.contains(SrcReg);
  //bool srcIsScalar8x64 = NaplesPU::VR512LRegClass.contains(SrcReg);
  
  unsigned operation;

  if (destIsScalar32 && srcIsScalar32)
    operation = NaplesPU::MOVE_SS_32;
  //else if (destIsScalar64 && srcIsScalar64)
  //  operation = NaplesPU::MOVE_SS_64;
  else if (destIsVector16x32 && srcIsScalar32)
    operation = NaplesPU::MOVE_VS_32;
  //else if (destIsVector8x64 && srcIsScalar64)
   // operation = NaplesPU::MOVE_VS_64;
  else if (destIsVector16x32 && srcIsScalar16x32)
    operation = NaplesPU::MOVE_VV_32;
  //else if (destIsVector8x64 && srcIsScalar8x64)
  //  operation = NaplesPU::MOVE_VV_64;
  else
    llvm_unreachable("unsupported physical reg copy type");

  BuildMI(MBB, I, DL, get(operation), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
}


/// Store the specified register of the given register class to the specified
/// stack frame index. The store instruction is to be added to the given
/// machine basic block before the specified machine instruction. If isKill
/// is true, the register operand is the last use and must be marked kill.
void NaplesPUInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC,
                    const TargetRegisterInfo *TRI) const {
  DebugLoc DL;
  if (I != MBB.end())
    DL = I->getDebugLoc();

  MachineMemOperand *MMO =
      getMemOperand(MBB, FI, MachineMemOperand::MOStore);
  unsigned Opc = 0;

  if (NaplesPU::GPR32RegClass.hasSubClassEq(RC))
    Opc = NaplesPU::S32W_Mainmem;
//  else if (NaplesPU::GPR64RegClass.hasSubClassEq(RC))
//    Opc = NaplesPU::S64L_Mainmem;
  else if (NaplesPU::VR512WRegClass.hasSubClassEq(RC))
    Opc = NaplesPU::SV32WMainmem_U;
//  else if (NaplesPU::VR512LRegClass.hasSubClassEq(RC))
//    Opc = NaplesPU::SV64LMainmem_U;
  else
    llvm_unreachable("unknown register class in storeRegToStack");

  BuildMI(MBB, I, DL, get(Opc))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI)
      .addImm(0)
      .addMemOperand(MMO);
}


/// Load the specified register of the given register class from the specified
/// stack frame index. The load instruction is to be added to the given
/// machine basic block before the specified machine instruction.

void NaplesPUInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC,
                     const TargetRegisterInfo *TRI) const {
   DebugLoc DL;
   if (I != MBB.end())
     DL = I->getDebugLoc();

   MachineMemOperand *MMO =
       getMemOperand(MBB, FI, MachineMemOperand::MOLoad);
   unsigned Opc = 0;

   if (NaplesPU::GPR32RegClass.hasSubClassEq(RC))
     Opc = NaplesPU::L32W_Mainmem;
  // else if (NaplesPU::GPR64RegClass.hasSubClassEq(RC))
    // Opc = NaplesPU::L64L_Mainmem;
   else if (NaplesPU::VR512WRegClass.hasSubClassEq(RC))
     Opc = NaplesPU::LV32WMainmem_U;
  // else if (NaplesPU::VR512LRegClass.hasSubClassEq(RC))
  //   Opc = NaplesPU::LV64LMainmem_U;
   else
     llvm_unreachable("unknown register class in storeRegToStack");

   BuildMI(MBB, I, DL, get(Opc), DestReg)
       .addFrameIndex(FI)
       .addImm(0)
       .addMemOperand(MMO);

}

void NaplesPUInstrInfo::adjustStackPointer(MachineBasicBlock &MBB,
                                        MachineBasicBlock::iterator MBBI,
                                        const DebugLoc &DL,
                                        int Amount) const {

  if (isInt<9>(Amount)) {
    // If the immediate value can be endcoded on 9 bits,
    // use ADD Immediate.
    BuildMI(MBB, MBBI, DL, get(NaplesPU::ADDSSI), NaplesPU::SP_REG)
        .addReg(NaplesPU::SP_REG)
        .addImm(Amount);
  } else {
    // Else load the Immediate and ADD it to the SP.
    unsigned int OffsetReg = loadConstant(MBB, MBBI, Amount);
    BuildMI(MBB, MBBI, DL, get(NaplesPU::ADDSSS_32))
        .addReg(NaplesPU::SP_REG)
        .addReg(NaplesPU::SP_REG)
        .addReg(OffsetReg);
  }
}

// Load constant larger than immediate field 
unsigned int NaplesPUInstrInfo::loadConstant(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MBBI,
                                          int Value) const {

  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  DebugLoc DL = MBBI->getDebugLoc();
  // Create a Virtual Register to store the constant
  unsigned Reg = RegInfo.createVirtualRegister(&NaplesPU::GPR32RegClass);
  if (!isInt<32>(Value))
    report_fatal_error("NaplesPUInstrInfo::loadConstant: value out of range");
  
  // Load the constant using the MOVEIH and MOVEIL instructions
  BuildMI(MBB, MBBI, DL, get(NaplesPU::MOVEIHSI), Reg).addImm(Value >> 16);

  if ((Value & 0xffff) != 0) {
    BuildMI(MBB, MBBI, DL, get(NaplesPU::MOVEILSI), Reg).addImm(Value & 0xffff);
  }

  return Reg;
}

// Usata solo per ottenere il frameindex come operando
MachineMemOperand *NaplesPUInstrInfo::getMemOperand(MachineBasicBlock &MBB, int FI,
                                                 MachineMemOperand::Flags Flag) const {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  unsigned Align = MFI.getObjectAlignment(FI);

  return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF, FI),
                                 Flag, MFI.getObjectSize(FI), Align);
}
