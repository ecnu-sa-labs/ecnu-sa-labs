#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

namespace instrument {
  void instrumentTrunc(Instruction &I,std::string deststr,std::string srcstr);

  void instrumentGEP(Instruction &I,std::string deststr,std::string srcstr);

  void instrumentStore(Instruction &I,std::string deststr,std::string srcstr);

  void instrumentLoad(Instruction &I,std::string deststr,std::string srcstr);

  void instrumentBinaryOperator(Instruction &I,std::string deststr,std::string srcstr1,std::string srcstr2);

  void instrumentTaintSinkVal(Instruction &I,std::string src);

  void instrumentTaintSinkPtrVal(Instruction &I,Value *var,std::string src);

  void instrumentTaintSourceVal(Instruction &I, std::string id);

  void instrumentTaintSourcePtrVal(Instruction &I, Value *var, std::string id);

  void instrumentForStrcat(Instruction &I, Value *dst, Value *src,std::string deststr,std::string srcstr);
  
  struct Instrument : public FunctionPass {
    static char ID;

    Instrument() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override;
  };
} // namespace instrument
