#include "NaplesPUTargetTransformInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
using namespace llvm;

unsigned NaplesPUTTIImpl::getNumberOfRegisters(bool Vec) const {
  return 32;
}

unsigned NaplesPUTTIImpl::getRegisterBitWidth(bool Vector) const {
  if (Vector)
    return 512;
  else
    return 32;
}

unsigned NaplesPUTTIImpl::getMinVectorRegisterBitWidth() const {
  return 512;
}

bool NaplesPUTTIImpl::shouldMaximizeVectorBandwidth(bool OptSize) const {
  return false;
}

unsigned NaplesPUTTIImpl::getMaxInterleaveFactor(unsigned VF) const {
  return 32;
}

bool NaplesPUTTIImpl::hasBranchDivergence() const {
  return false;
}
