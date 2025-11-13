#include "Instrument.h"
#include "Utils.h"

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "DynTaintAnalysisPass";
const auto PASS_DESC = "dyntaint-pass";

void setTaintSource(Instruction &I);
void setTaintSink(Instruction &I);
void setTaintPropagation(Instruction &I);

/**
 * @brief Instruments given function for dynamic analysis.
 *
 * @param F Function to be instrumented.
 * @return true indicates that function has been modified.
 */
bool Instrument::runOnFunction(Function &F) {
	auto FunctionName = F.getName().str();

	Module *M = F.getParent();
	outs() << "Running TaintPass on module: " << M->getName() << "\n";

	for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
		Instruction &Inst = (*Iter);

		std::string ResultString = variable(&Inst);
		std::string FirstOperandString;
		std::string SecondOperandString;
	
		getOperandsString(Inst, FirstOperandString, SecondOperandString);

		//For function calls, check whether the function is a taint source or a taint sink or a taint propagation function
		if (isa<CallInst>(&Inst)) {
			setTaintSource(Inst);
			setTaintSink(Inst);
			setTaintPropagation(Inst);
		}


		/**
		* TODO: Add code to instrument the BinaryOperator/LoadInst/StoreInst/GetElementPtrInst/TruncInst
		* to record their runtime information and propagate the taint.
		* You can get instrument functions' signature from /lab7/include/Instrument.h.
		* When instrument Store, pay extra attention to who is the source/destination operand.
		*/
		
	}
	return true;
}


/**
 * @brief For function scanf() and getchar(), set taint source.
 *
 * @param I Instruction to be instrumented.
 */
void setTaintSource(Instruction &I){
	CallInst * call = dyn_cast<CallInst>(&I);
	Function *calledFunc = call->getCalledFunction();


	/**
	 * TODO: Add code to instrument scanf() and getchar() to mark
	 * their parameter as taint.
	 * 
	 * Note: Consider which parameters/return-results need to be tainted!
	 */
	

}


/**
 * @brief For functioin system() and check_secret(), check wether the parameter is tainted
 *
 * @param I Instruction to be instrumented.
 */
void setTaintSink(Instruction &I){
	CallInst * call = dyn_cast<CallInst>(&I);
	Function *calledFunc = call->getCalledFunction();

	if (calledFunc && calledFunc->getName().contains("system")) {
		Value *para = call->getArgOperand(0);
		instrumentTaintSinkPtrVal(I,para,variable(para));
	}
	if (calledFunc && calledFunc->getName().contains("check_secret")) {
		Value *para = call->getArgOperand(0);
		instrumentTaintSinkVal(I,variable(para));
	}
}


/**
 * @brief For function strcat(), propagate the taint from src to dest
 *
 * @param I Instruction to be instrumented.
 */
void setTaintPropagation(Instruction &I){
	CallInst * call = dyn_cast<CallInst>(&I);
	Function *calledFunc = call->getCalledFunction();
	
	if (calledFunc && calledFunc->getName().contains("strcat")) {
		Value *dest = call->getArgOperand(0);
		Value *src = call->getArgOperand(1);
		
		instrumentForStrcat(I, dest, src,variable(dest),variable(src)); 
	}
}


char Instrument::ID = 1;
static RegisterPass<Instrument> X(PASS_NAME, PASS_NAME, false, false);

} // namespace instrument
