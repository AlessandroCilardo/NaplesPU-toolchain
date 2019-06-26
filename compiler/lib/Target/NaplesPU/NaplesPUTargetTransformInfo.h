#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "NaplesPUTargetMachine.h"

namespace llvm {
class NaplesPUTTIImpl final : public BasicTTIImplBase<NaplesPUTTIImpl> {
  typedef BasicTTIImplBase<NaplesPUTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const NaplesPUSubtarget *ST;
  const NaplesPUTargetLowering *TLI;

  const NaplesPUSubtarget *getST() const { return ST; }
  const NaplesPUTargetLowering *getTLI() const { return TLI; }

public:
  explicit NaplesPUTTIImpl(const NaplesPUTargetMachine *TM, const Function &F)
    : BaseT(TM, F.getParent()->getDataLayout()),
      ST(TM->getSubtargetImpl(F)),
      TLI(ST->getTargetLowering()) {}

  unsigned getNumberOfRegisters(bool Vector) const;
  unsigned getRegisterBitWidth(bool Vector) const;
  unsigned getMinVectorRegisterBitWidth() const;
  bool shouldMaximizeVectorBandwidth(bool OptSize) const;
  unsigned getMaxInterleaveFactor(unsigned VF) const;
  bool hasBranchDivergence() const;
};
}
