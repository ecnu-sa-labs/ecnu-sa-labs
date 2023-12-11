/**
 * @file Runtime.cpp
 * @brief Symbolically evaluate instruction using callback functions.
 */

#include <iostream>

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"

#include "SymbolicInterpreter.h"

extern SymbolicInterpreter SI;

/**
 * @brief Symbolically evaluate given expression.
 *
 * @param E Z3 expression to be evaluated.
 * @return Evaluated Z3 expression.
 */
z3::expr eval(z3::expr &E) {
  if (E.kind() == Z3_NUMERAL_AST) {
    return E;
  } else {
    MemoryTy Mem = SI.getMemory();
    Address Register(E);
    if (Mem.find(Register) != Mem.end()) {
      return Mem.at(Register);
    } else {
      std::cout << "Warning: Cannot find register " << Register << " in memory "
                << std::endl;
      return E;
    }
  }
}

/**
 * @brief Symbolically evaluate Alloca instruction.
 *
 * @param R Register ID.
 * @param Ptr Address.
 */
extern "C" void __DSE_Alloca__(int R, int *Ptr) {
  MemoryTy &Mem = SI.getMemory();
  Address Register(R);
  z3::expr SE = SI.getContext().int_val((uintptr_t)Ptr);
  Mem.insert(std::make_pair(Register, SE));
}

/**
 * @brief Symbolically evaluate Store instruction.
 *
 * @param X Address.
 */
extern "C" void __DSE_Store__(int *X) {
  MemoryTy &Mem = SI.getMemory();
  Address Addr(X);
  z3::expr SE = eval(SI.getStack().top());
  SI.getStack().pop();
  Mem.erase(Addr);
  Mem.insert(std::make_pair(Addr, SE));
}

/**
 * @brief Symbolically evaluate Load instruction.
 *
 * TODO: Implement this.
 *
 * Hint: int y = *x;
 *
 * @param Y Address of destination.
 * @param X Address of load source.
 */
extern "C" void __DSE_Load__(int Y, int *X) {
}

/**
 * @brief Symbolically evaluate Comparision instruction.
 *
 * TODO: Implement this.
 *
 * @param R Address of comparision result.
 * @param Op Operator kind.
 */
extern "C" void __DSE_ICmp__(int R, int Op) {
}

/**
 * @brief Symbolically evaluate Binary Operator.
 *
 * TODO: Implement this.
 *
 * @param R Address of binary operation result.
 * @param Op Operator kind.
 */
extern "C" void __DSE_BinOp__(int R, int Op) {
}
