#include "Instrument.h"
#include "Utils.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "StaticAnalysisPass";
const auto PASS_DESC = "Static Analysis Pass";

bool Instrument::runOnFunction(Function &F) {
  auto FunctionName = F.getName().str();
  outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";
  outs() << "Locating Instructions\n";

  for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
    Instruction &Inst = (*Iter);
    llvm::DebugLoc DebugLoc = Inst.getDebugLoc();

    if (!DebugLoc) {
      // Skip Instruction if it doesn't have debug information.
      continue;
    }

    int Line = DebugLoc.getLine();
    int Col = DebugLoc.getCol();
    outs() << Line << ", " << Col << "\n";

    /**
     * TODO: Add code to check if instruction is a BinaryOperator and if so,
     * print information about its location and operands as specified in Lab
     * document.
     */
  }
  return false;
}

char Instrument::ID = 1;

struct StaticAnalysisNPMWrapper : public PassInfoMixin<StaticAnalysisNPMWrapper> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    Instrument P;
    bool Modified = P.runOnFunction(F);
    return Modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION,
      PASS_NAME,
      "0.1",
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "static-analysis" || Name == PASS_NAME) {
                FPM.addPass(StaticAnalysisNPMWrapper());
                return true;
              }
              return false;
            });
        PB.registerPipelineStartEPCallback(
            [](ModulePassManager &MPM, OptimizationLevel) {
              MPM.addPass(
                  createModuleToFunctionPassAdaptor(StaticAnalysisNPMWrapper()));
            });
      }};
}

} // namespace instrument