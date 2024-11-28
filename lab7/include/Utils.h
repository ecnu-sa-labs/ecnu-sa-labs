#ifndef UTILS_H
#define UTILS_H

#include "llvm/IR/Instruction.h"

using namespace llvm;

/**
 * Get human-readable string name for operands in an instruction.
 */
void getOperandsString(Instruction &I, std::string &op1, std::string &op2);

/**
 * Get a human-readable string name for an llvm Value.
 */
std::string variable(Value *V);

#endif // UTILS_H
