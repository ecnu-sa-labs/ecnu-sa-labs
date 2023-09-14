#include "PointerAnalysis.h"
#include "Utils.h"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"

namespace dataflow {

void PointerAnalysis::transfer(Instruction *Inst, PointsToInfo &PointsTo) {
  if (AllocaInst *Alloca = dyn_cast<AllocaInst>(Inst)) {
    std::string Pointer = variable(Alloca);
    PointsToSet &S = PointsTo[variable(Alloca)];
    S.insert(address(Alloca));
  } else if (StoreInst *Store = dyn_cast<StoreInst>(Inst)) {
    if (!Store->getValueOperand()->getType()->isPointerTy())
      return;
    Value *Pointer = Store->getPointerOperand();
    Value *Value = Store->getValueOperand();
    PointsToSet &L = PointsTo[variable(Pointer)];
    PointsToSet &R = PointsTo[variable(Value)];
    for (auto &I : L) {
      PointsToSet &S = PointsTo[I];
      PointsToSet Result;
      std::set_union(S.begin(), S.end(), R.begin(), R.end(),
                     std::inserter(Result, Result.begin()));
      PointsTo[I] = Result;
    }
  } else if (LoadInst *Load = dyn_cast<LoadInst>(Inst)) {
    if (!Load->getType()->isPointerTy())
      return;
    std::string Variable = variable(Load->getPointerOperand());
    PointsToSet &R = PointsTo[Variable];
    PointsToSet &L = PointsTo[variable(Load)];
    PointsToSet Result;
    for (auto &I : R) {
      PointsToSet &S = PointsTo[I];
      std::set_union(S.begin(), S.end(), Result.begin(), Result.end(),
                     std::inserter(Result, Result.begin()));
    }
    PointsTo[variable(Load)] = Result;
  }
}

int PointerAnalysis::countFacts(PointsToInfo &PointsTo) {
  int N = 0;
  for (auto &I : PointsTo)
    N += I.second.size();
  return N;
}

void PointerAnalysis::print(std::map<std::string, PointsToSet> &PointsTo) {
  errs() << "Pointer Analysis Results:\n";
  for (auto &I : PointsTo) {
    errs() << "  " << I.first << ": { ";
    for (auto &J : I.second) {
      errs() << J << "; ";
    }
    errs() << "}\n";
  }
  errs() << "\n";
}

PointerAnalysis::PointerAnalysis(Function &F) {
  int NumOfOldFacts = 0;
  int NumOfNewFacts = 0;

  while (true) {
    for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
      auto Inst = &*Iter;
      transfer(Inst, PointsTo);
    }
    NumOfNewFacts = countFacts(PointsTo);
    if (NumOfOldFacts < NumOfNewFacts)
      NumOfOldFacts = NumOfNewFacts;
    else
      break;
  }
  print(PointsTo);
}

bool PointerAnalysis::alias(std::string &Ptr1, std::string &Ptr2) const {
  if (PointsTo.find(Ptr1) == PointsTo.end() ||
      PointsTo.find(Ptr2) == PointsTo.end())
    return false;
  const PointsToSet &S1 = PointsTo.at(Ptr1);
  const PointsToSet &S2 = PointsTo.at(Ptr2);

  PointsToSet Inter;
  std::set_intersection(S1.begin(), S1.end(), S2.begin(), S2.end(),
                        std::inserter(Inter, Inter.begin()));
  return !Inter.empty();
}

}; // namespace dataflow