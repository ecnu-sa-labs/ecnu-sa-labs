#include "Instrument.h"
#include "Utils.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "DynamicAnalysisPass";
const auto PASS_DESC = "Dynamic Analysis Pass";

const auto COVERAGE_FUNCTION_NAME = "__coverage__";
const auto BINOP_OPERANDS_FUNCTION_NAME = "__binop_op__";

void instrumentCoverage(Module *M, Instruction &I, int Line, int Col);
void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line,
                             int Col);

/**
 * @brief Instruments given function for dynamic analysis.
 *
 * @param F Function to be instrumented.
 * @return true indicates that function has been modified.
 */
bool Instrument::runOnFunction(Function &F) {
  auto FunctionName = F.getName().str();
  outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";
  outs() << "Instrument Instructions\n";

  LLVMContext &Context = F.getContext();
  Module *M = F.getParent();

  Type *VoidType = Type::getVoidTy(Context);
  Type *Int32Type = Type::getInt32Ty(Context);
  Type *Int8Type = Type::getInt8Ty(Context);

  (void)M->getOrInsertFunction(
      COVERAGE_FUNCTION_NAME,
      FunctionType::get(VoidType, {Int32Type, Int32Type}, /*isVarArg=*/false));
  (void)M->getOrInsertFunction(
      BINOP_OPERANDS_FUNCTION_NAME,
      FunctionType::get(VoidType,
                        {Int8Type, Int32Type, Int32Type, Int32Type, Int32Type},
                        /*isVarArg=*/false));

  for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
    Instruction &Inst = (*Iter);
    llvm::DebugLoc DebugLoc = Inst.getDebugLoc();

    if (!DebugLoc) {
      // Skip Instruction if it doesn't have debug information.
      continue;
    }

    int Line = DebugLoc.getLine();
    int Col = DebugLoc.getCol();
    instrumentCoverage(M, Inst, Line, Col);

    /**
     * TODO: Add code to check if instruction is a BinaryOperator and if so,
     * instrument instruction as specified in Lab document.
     */
  }

  return true;
}

/**
 * @brief Instruments given instruction for coverage.
 *
 * @param M LLVM module.
 * @param I Instruction to be instrumented.
 * @param Line Line of instructions.
 * @param Col Column of instruction.
 */
void instrumentCoverage(Module *M, Instruction &I, int Line, int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);

  // Convert line and column numbers to LLVM values.
  auto LineVal = ConstantInt::get(Int32Type, Line);
  auto ColVal = ConstantInt::get(Int32Type, Col);

  std::vector<Value *> Args = {LineVal, ColVal};

  auto *CoverageFunction = M->getFunction(COVERAGE_FUNCTION_NAME);
  CallInst::Create(CoverageFunction, Args, "", &I);
}

/**
 * @brief Instruments given binary operator for its operands.
 *
 * @param M LLVM module.
 * @param BinOp Binary operator to be instrumented.
 * @param Line Line of binary operators.
 * @param Col Column of binary operator.
 */
void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line,
                             int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);
  auto *CharType = Type::getInt8Ty(Context);

  /**
   * TODO: Add code to instrument the BinaryOperator to print
   * its location, operation type and the runtime values of its
   * operands.
   */
}

char Instrument::ID = 1;
static RegisterPass<Instrument> X(PASS_NAME, PASS_NAME, false, false);


struct DynamicAnalysisNPMWrapper : public PassInfoMixin<DynamicAnalysisNPMWrapper> {
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
              if (Name == "dynamic-analysis" || Name == PASS_NAME) {
                FPM.addPass(DynamicAnalysisNPMWrapper());
                return true;
              }
              return false;
            });
        PB.registerPipelineStartEPCallback(
            [](ModulePassManager &MPM, OptimizationLevel) {
              MPM.addPass(
                  createModuleToFunctionPassAdaptor(DynamicAnalysisNPMWrapper()));
            });
      }};
}

} // namespace instrument