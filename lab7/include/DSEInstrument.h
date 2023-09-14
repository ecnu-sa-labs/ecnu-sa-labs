#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace dse {

int RegisterID = 0;
std::map<Value *, int> RegisterMap;
int BranchID = 0;
std::map<Instruction *, int> BranchMap;

int getRegisterID(Value *I) {
  if (RegisterMap.find(I) == RegisterMap.end()) {
    RegisterMap[I] = RegisterMap.size();
  }
  return RegisterMap[I];
}

int getBranchID(Instruction *I) {
  if (BranchMap.find(I) == BranchMap.end()) {
    BranchMap[I] = BranchMap.size();
  }
  return BranchMap[I];
}

struct DSEInstrument : public FunctionPass {
  static char ID;
  static const char *checkFunctionName;

  DSEInstrument() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;
};
} // namespace dse
