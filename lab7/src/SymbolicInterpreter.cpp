/**
 * @file SymbolicInterpreter.cpp
 * @brief Implement callback functions for symbolic interpretation.
 */

#include "SymbolicInterpreter.h"

#include <ctime>
#include <fstream>

/**
 * @brief Overload output stream operator for Address printing.
 *
 * @param OS Output stream.
 * @param A Address to be printed.
 * @return Modified output stream.
 */
std::ostream &operator<<(std::ostream &OS, const Address &A) {
  if (A.Type == A.Memory) {
    OS << A.Addr;
  } else {
    OS << "R" << A.Addr;
  }
  return OS;
}

/**
 * @brief Create new input for Symbolic Interpreter.
 *
 * @param Ptr Ptr to input.
 * @param ID ID for inputs.
 * @return New input value.
 */
int SymbolicInterpreter::NewInput(int *Ptr, int ID) {
  int Ret = 0;
  if (Inputs.find(ID) != Inputs.end()) {
    Ret = Inputs[ID];
  } else {
    Ret = std::rand();
    Inputs[ID] = Ret;
  }

  Address X(Ptr);
  std::string InputName = "X" + std::to_string(NumOfInputs);
  z3::expr SE = Ctx.int_const(InputName.c_str());
  Mem.insert(std::make_pair(X, SE));
  NumOfInputs++;

  return Ret;
}

SymbolicInterpreter SI;

/**
 * @brief Print inputs, symbolic memory, and path condition.
 *
 * @param OS Output stream.
 */
void print(std::ostream &OS) {
  OS << "=== Inputs ===" << std::endl;
  for (auto &E : SI.getInputs()) {
    OS << "X" << E.first << " : " << E.second << std::endl;
  }
  OS << std::endl;

  OS << "=== Symbolic Memory ===" << std::endl;
  for (auto &E : SI.getMemory()) {
    OS << E.first << " : " << E.second << std::endl;
  }
  OS << std::endl;

  OS << "=== Path Condition ===" << std::endl;
  for (auto &E : SI.getPathCondition()) {
    std::string BID = "B" + std::to_string(E.first);
    OS << BID << " : " << E.second << std::endl;
  }
  OS << std::endl;
}

/**
 * @brief Handle exit of DSE.
 */
extern "C" void __DSE_Exit__() {
  z3::solver Solver(SI.getContext());
  std::ofstream Branch(BranchFile);

  for (auto &E : SI.getPathCondition()) {
    std::string BID = "B" + std::to_string(E.first);
    Solver.add(E.second);
    Branch << BID << "\n";
  }

  std::ofstream Smt2(FormulaFile);
  Smt2 << Solver.to_smt2();
  std::ofstream Log(LogFile);
  print(Log);
}

/**
 * @brief Handle init of DSE.
 */
extern "C" void __DSE_Init__() {
  std::srand(std::time(nullptr));
  std::string Line;
  std::ifstream Input(InputFile);

  if (Input.is_open()) {
    while (getline(Input, Line)) {
      int ID = std::stoi(Line.substr(1, Line.find(",")));
      int Val = std::stoi(Line.substr(Line.find(",") + 1));
      SI.getInputs()[ID] = Val;
    }
  }

  std::atexit(__DSE_Exit__);
}

/**
 * @brief Handle DSE input.
 *
 * @param X Pointer to input value.
 * @param ID ID for input.
 */
extern "C" void __DSE_Input__(int *X, int ID) { *X = (int)SI.NewInput(X, ID); }

/**
 * @brief Handle DSE branching.
 *
 * @param BID Branch ID.
 * @param RID Register ID.
 * @param BIF Branch condition.
 */
extern "C" void __DSE_Branch__(int BID, int RID, int BIF) {
  MemoryTy &Mem = SI.getMemory();
  Address Addr(RID);
  z3::expr SE = Mem.at(Addr);
  z3::expr Cond =
      BIF ? SI.getContext().bool_val(true) : SI.getContext().bool_val(false);
  SI.getPathCondition().push_back(std::make_pair(BID, SE == Cond));
}

/**
 * @brief Handle DSE constant value.
 *
 * @param X Constant value.
 */
extern "C" void __DSE_Const__(int X) {
  z3::expr SE = SI.getContext().int_val(X);
  SI.getStack().push(SE);
}

/**
 * @brief Handle DSE register value.
 *
 * @param X Register value.
 */
extern "C" void __DSE_Register__(int X) {
  std::string Name = "R" + std::to_string(X);
  z3::expr SE = SI.getContext().int_const(Name.c_str());
  SI.getStack().push(SE);
}