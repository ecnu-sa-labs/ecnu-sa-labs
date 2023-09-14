/**
 * @file Transfer.cpp
 * @brief Evaluating instruction using abstract states.
 */

#include "DivZeroAnalysis.h"
#include "Utils.h"

namespace dataflow {

/**
 * @brief Is given instruction a user input?
 *
 * @param Inst Instruction to check.
 * @return true if it is a user input, false otherwise.
 */
bool isInput(Instruction *Inst) {
  if (auto Call = dyn_cast<CallInst>(Inst)) {
    if (auto Fun = Call->getCalledFunction()) {
      return (Fun->getName().equals("getchar") ||
              Fun->getName().equals("fgetc"));
    }
  }
  return false;
}

/**
 * @brief Evaluate PHINode to get its Domain.
 *
 * @param Phi PHINode to evaluate.
 * @param InMem InMemory of PHINode.
 * @return Domain of PHINode.
 */
Domain *eval(PHINode *Phi, const Memory *InMem) {
  if (auto ConstantVal = Phi->hasConstantValue()) {
    return new Domain(extractFromValue(ConstantVal));
  }

  Domain *Joined = new Domain(Domain::Uninit);

  for (unsigned int i = 0; i < Phi->getNumIncomingValues(); i++) {
    auto Dom = getOrExtract(InMem, Phi->getIncomingValue(i));
    Joined = Domain::join(Joined, Dom);
  }

  return Joined;
}

/**
 * @brief Evaluate +, -, * and / BinaryOperator instructions
 * using Domain of its operands and return Domain of result.
 *
 * @param BinOp BinaryOperator to evaluate.
 * @param InMem InMemory of BinOp.
 * @return Domain of BinOp.
 */
Domain *eval(BinaryOperator *BinOp, const Memory *InMem) {
  /**
   * TODO: Write your code here that evaluates +, -, * and /
   * based on Domains of operands.
   */
  return NULL;
}

/**
 * @brief Evaluate Cast instructions.
 *
 * @param Cast Cast instruction to evaluate.
 * @param InMem InMemory of Cast.
 * @return Domain of Cast.
 */
Domain *eval(CastInst *Cast, const Memory *InMem) {
  /**
   * TODO: Write your code here to evaluate Cast instruction.
   */
  return NULL;
}

/**
 * @brief Evaluate ==, !=, <, <=, >=, and > Comparision operators using
 * Domain of its operands to compute Domain of result.
 *
 * @param Cmp Comparision instruction to evaluate.
 * @param InMem InMemory of Cmp.
 * @return Domain of Cmp.
 */
Domain *eval(CmpInst *Cmp, const Memory *InMem) {
  /**
   * TODO: Write your code here that evaluates:
   * ==, !=, <, <=, >=, and > based on Domains of operands.
   *
   * NOTE: There is a lot of scope for refining this, but you can just return
   * MaybeZero for comparisons other than equality.
   */
  return NULL;
}

/**
 * @brief Main function of Transfer.cpp.
 *
 * @param Inst Instruction to evaluate.
 * @param In InMemory.
 * @param NOut OutMemory.
 */
void DivZeroAnalysis::transfer(Instruction *Inst, const Memory *In,
                               Memory &NOut) {
  if (isInput(Inst)) {
    NOut[variable(Inst)] = new Domain(Domain::Element::MaybeZero);
  } else if (auto Phi = dyn_cast<PHINode>(Inst)) {
    NOut[variable(Phi)] = eval(Phi, In);
  } else if (auto BinOp = dyn_cast<BinaryOperator>(Inst)) {
    NOut[variable(BinOp)] = eval(BinOp, In);
  } else if (auto Cast = dyn_cast<CastInst>(Inst)) {
    NOut[variable(Cast)] = eval(Cast, In);
  } else if (auto Cmp = dyn_cast<CmpInst>(Inst)) {
    NOut[variable(Cmp)] = eval(Cmp, In);
  }
}

} // namespace dataflow