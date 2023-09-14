#ifndef UTILS_H
#define UTILS_H

#include "llvm/IR/Instruction.h"

using namespace llvm;

/**
 * Get a symbol corresponding to OpCode of a Binary Operator.
 *
 * Multiplication -> *
 * Division       -> /
 * Modulo         -> %
 * Addition       -> +
 * Subtraction    -> -
 */
char getBinOpSymbol(Instruction::BinaryOps Opcode);

/**
 * Get haman-friendly string name of a Binary operator from its symbol.
 */
const std::string getBinOpName(const char symbol);

/**
 * Get a human-readable string name for an llvm Value.
 */
std::string variable(Value *V);

#endif // UTILS_H
