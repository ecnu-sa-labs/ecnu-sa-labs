#ifndef SYMBOLIC_INTERPRETER_H
#define SYMBOLIC_INTERPRETER_H

#include <cstdlib>
#include <map>
#include <stack>
#include <vector>

#include "z3++.h"

static const char *FormulaFile = "formula.smt2";
static const char *InputFile = "input.txt";
static const char *LogFile = "log.txt";
static const char *BranchFile = "branch.txt";

class Address {
public:
  enum Ty { Memory, Register };
  Address(int *Ptr) : Type(Memory), Addr((uintptr_t)Ptr) {}
  Address(int ID) : Type(Register), Addr(ID) {}
  Address(z3::expr &R)
      : Type(Register), Addr(std::stoi(R.to_string().substr(1))) {}

  bool operator<(const Address &RHS) const {
    return (Type < RHS.Type) || (Type == RHS.Type && Addr < RHS.Addr);
  }
  friend std::ostream &operator<<(std::ostream &OS, const Address &SE);

private:
  Ty Type;
  uintptr_t Addr;
};

using MemoryTy = std::map<Address, z3::expr>;

class SymbolicInterpreter {
public:
  int NewInput(int *Ptr, int ID);
  MemoryTy &getMemory() { return Mem; }
  std::map<int, int> &getInputs() { return Inputs; }
  z3::context &getContext() { return Ctx; }
  std::stack<z3::expr> &getStack() { return Stack; }
  std::vector<std::pair<int, z3::expr>> &getPathCondition() {
    return PathCondition;
  }

private:
  MemoryTy Mem;
  std::map<int, int> Inputs;
  int NumOfInputs = 0;
  std::stack<z3::expr> Stack;
  std::vector<std::pair<int, z3::expr>> PathCondition;
  z3::context Ctx;
};

#endif // SYMBOLIC_INTERPRETER_H
