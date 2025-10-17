/**
 * @file Instrument.cpp
 * @brief Instrumentation for dynamic analysis.
 */

#include "Instrument.h"


#include "llvm/Passes/OptimizationLevel.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "InstrumentPass";

// Names of functions that will be used for instrumentation.
static const char *SANITIZE_FUNCTION_NAME = "__sanitize__";
static const char *COVERAGE_FUNCTION_NAME = "__coverage__";

/**
 * @brief Instruments given instruction with coverage logging.
 *
 * @param M Module containing instruction.
 * @param I Instruction to instrument.
 * @param Line Line of instructions.
 * @param Col Column of instruction.
 */
void instrumentCoverage(Module *M, Instruction &I, int Line, int Col) {
  // Get context of module.
  auto &Context = M->getContext();

  // Define a 32-bit integer type.
  Type *Int32Type = Type::getInt32Ty(Context);

  // Create constant integer values for line and column numbers.
  auto *LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto *ColVal = llvm::ConstantInt::get(Int32Type, Col);

  // Add line and column numbers to arguments list.
  std::vector<Value *> Args = {LineVal, ColVal};

  // Get a reference to coverage function.
  auto *Fun = M->getFunction(COVERAGE_FUNCTION_NAME);

  // Insert a call to coverage function before instruction I.
  CallInst::Create(Fun, Args, "", &I);
}

/**
 * @brief Instruments given instruction with sanitization.
 *
 * @param M Module containing instruction.
 * @param I Instruction to instrument.
 * @param Line Line of instructions.
 * @param Col Column of instruction.
 */
void instrumentSanitize(Module *M, Instruction &I, int Line, int Col) {
  // Get context of module.
  LLVMContext &Context = M->getContext();

  // Define a 32-bit integer type.
  Type *Int32Type = Type::getInt32Ty(Context);

  // Retrieve divisor operand from instruction (e.g., for division).
  auto *Divisor = I.getOperand(1);

  // Create constant integer values for line and column numbers.
  auto *LineVal = llvm::ConstantInt::get(Int32Type, Line);
  auto *ColVal = llvm::ConstantInt::get(Int32Type, Col);

  // Add divisor, line, and column numbers to arguments list.
  std::vector<Value *> Args = {Divisor, LineVal, ColVal};

  // Get a reference to sanitization function.
  auto *Fun = M->getFunction(SANITIZE_FUNCTION_NAME);

  // Insert a call to sanitization function before instruction I.
  CallInst::Create(Fun, Args, "", &I);
}

/**
 * @brief Main function to instrument given function for dynamic analysis.
 *
 * @param F Function to instrument.
 * @return true if function was modified, false otherwise.
 */
bool Instrument::runOnFunction(Function &F) {
  // Get context and parent module of function.
  LLVMContext &Context = F.getContext();
  Module *M = F.getParent();

  // Define void and 32-bit integer types.
  Type *VoidType = Type::getVoidTy(Context);
  Type *Int32Type = Type::getInt32Ty(Context);

  // Insert or get references to coverage and sanitization functions.
  M->getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type,
                         Int32Type);
  M->getOrInsertFunction(SANITIZE_FUNCTION_NAME, VoidType, Int32Type, Int32Type,
                         Int32Type);

  // Iterate over each instruction in function.
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    // Skip PHI nodes as they are not actual executable instructions.
    if (I->getOpcode() == Instruction::PHI) {
      continue;
    }

    // Get debug location for instruction.
    const auto DebugLoc = I->getDebugLoc();
    if (!DebugLoc) {
      continue;
    }

    // Extract line and column information for instruction.
    int Line = DebugLoc.getLine();
    int Col = DebugLoc.getCol();

    // If instruction is a signed or unsigned division, instrument it for
    // sanitization.
    if (I->getOpcode() == Instruction::SDiv ||
        I->getOpcode() == Instruction::UDiv) {
      instrumentSanitize(M, *I, Line, Col);
    }

    instrumentCoverage(M, *I, Line, Col);
  }

  return true;
}

char Instrument::ID = 1;
static RegisterPass<Instrument>
X(PASS_NAME, PASS_NAME, false, false);


struct InstrumentNPMWrapper : public PassInfoMixin<InstrumentNPMWrapper> {
  PreservedAnalyses run(Function& F, FunctionAnalysisManager&) {
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
      [](PassBuilder& PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager& FPM,
              ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == PASS_NAME) {
                  FPM.addPass(InstrumentNPMWrapper());
                return true;
              }
              return false;
            });
        // Optional: Register for other pipeline points if needed
        PB.registerPipelineStartEPCallback(
            [](ModulePassManager& MPM, OptimizationLevel) {
              MPM.addPass(createModuleToFunctionPassAdaptor(InstrumentNPMWrapper()));
            });
      } };
}

} // namespace instrument
