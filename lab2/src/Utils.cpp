#include "Utils.h"

#include "llvm/IR/Instruction.h"

using namespace llvm;

const char *WHITESPACES = " \t\n\r";

char getBinOpSymbol(Instruction::BinaryOps Opcode) {
  switch (Opcode) {
  case Instruction::Add:
  case Instruction::FAdd:
    return '+';
  case Instruction::Sub:
  case Instruction::FSub:
    return '-';
  case Instruction::Mul:
  case Instruction::FMul:
    return '*';
  case Instruction::UDiv:
  case Instruction::SDiv:
  case Instruction::FDiv:
    return '/';
  case Instruction::URem:
  case Instruction::SRem:
  case Instruction::FRem:
    return '%';
  default:
    return '?';
  }
}

const std::string getBinOpName(const char symbol) {
  switch (symbol) {
  case '+':
    return "Addition";
  case '-':
    return "Subtraction";
  case '*':
    return "Multiplication";
  case '/':
    return "Division";
  case '%':
    return "Modulo";
  default:
    return "Unknown operation";
  }
}

std::string variable(Value *V) {
  std::string Code;
  raw_string_ostream SS(Code);
  V->print(SS);
  Code.erase(0, Code.find_first_not_of(WHITESPACES));
  auto RetVal = Code.substr(0, Code.find_first_of(WHITESPACES));
  if (RetVal == "i32") {
    return Code.substr(Code.find_first_of(WHITESPACES) + 1, Code.length());
  }
  return RetVal;
}
