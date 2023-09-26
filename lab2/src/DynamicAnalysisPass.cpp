#include "Instrument.h"
#include "Utils.h"

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

  M->getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type,
                         Int32Type);
  M->getOrInsertFunction(BINOP_OPERANDS_FUNCTION_NAME, VoidType, Int8Type,
                         Int32Type, Int32Type, Int32Type, Int32Type);

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

  // Get symbol for binary operator.
  char operator1 = getBinOpSymbol(BinOp->getOpcode());

  // Convert line, column, and operator to LLVM values.
  auto OperatorVal = ConstantInt::get(CharType, operator1);
  auto LineVal = ConstantInt::get(Int32Type, Line);
  auto ColVal = ConstantInt::get(Int32Type, Col);

  // Get operands of binary operator.
  auto op1 = BinOp->getOperand(0);
  auto op2 = BinOp->getOperand(1);

  std::vector<Value *> Args = {OperatorVal, LineVal, ColVal, op1, op2};

  auto *BinOpsFunction = M->getFunction(BINOP_OPERANDS_FUNCTION_NAME);
  CallInst::Create(BinOpsFunction, Args, "", BinOp);
}

char Instrument::ID = 1;
static RegisterPass<Instrument> X(PASS_NAME, PASS_NAME, false, false);

} // namespace instrument
