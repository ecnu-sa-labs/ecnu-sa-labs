#include "Utils.h"

#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void getOperandsString(Instruction &I, std::string &op1, std::string &op2){
	for (unsigned i = 0; i < I.getNumOperands(); ++i) {
		if (i == 0) {
			op1 = variable(I.getOperand(i)); 
		} else if (i == 1) {
			op2 = variable(I.getOperand(i)); 
		}
	}
}

std::string variable(Value *V){
	if (V) {
	    std::string str;
	    raw_string_ostream rso(str);
	    V->printAsOperand(rso, false); 
	    return rso.str(); 
	}
	return ""; 
}
