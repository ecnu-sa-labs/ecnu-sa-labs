/**
 * @file DSE.cpp
 * @brief Main driver for Dynamic Symbolic Executor.
 */

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#include "Strategy.h"
#include "SymbolicInterpreter.h"
#include "z3++.h"

/**
 * @def ARG_EXIST_CHECK(Name, Arg)
 * @brief Verify file existence and assign its name to a string.
 *
 * @param Name String to hold file name.
 * @param Arg File name to check.
 */
#define ARG_EXIST_CHECK(Name, Arg)                                             \
  {                                                                            \
    struct stat Buffer;                                                        \
    if (stat(Arg, &Buffer)) {                                                  \
      fprintf(stderr, "%s not found\n", Arg);                                  \
      return 1;                                                                \
    }                                                                          \
  }                                                                            \
  std::string Name(Arg);

z3::context Ctx;
z3::solver Solver(Ctx);

/**
 * @brief Store symbolic input values from solver's model to InputFile.
 */
void storeInput() {
  std::ofstream OS(InputFile);
  z3::model Model = Solver.get_model();

  for (int I = 0; I < Model.size(); I++) {
    const z3::func_decl E = Model[I];
    z3::expr Input = Model.get_const_interp(E);

    if (Input.kind() == Z3_NUMERAL_AST) {
      int I = Input.get_numeral_int();
      OS << E.name() << "," << I << std::endl;
    }
  }
}

/**
 * @brief Log new path conditions to LogFile.
 *
 * @param Vec Path conditions vector.
 */
void printNewPathCondition(z3::expr_vector &Vec) {
  std::ofstream Log;
  Log.open(LogFile, std::ofstream::out | std::ofstream::app);
  Log << "\n=== New Path Condition ===\n";

  for (auto E : Vec) {
    Log << E << "\n";
  }
}

/**
 * @brief Generate new inputs by solving path conditions and
 * save them to InputFile.
 */
void generateInput() {
  z3::expr_vector Vec = Ctx.parse_file(FormulaFile);

  while (true) {
    searchStrategy(Vec);
    std::cout << "Selected path condition: \n";

    for (const auto &E : Vec) {
      std::cout << E << "\n";
      Solver.add(E);
    }

    z3::check_result Result = Solver.check();

    if (Result == z3::sat) {
      storeInput();
      printNewPathCondition(Vec);
      break;
    }
  }
}

/**
 * @brief Main function.
 * Usage: ./dse [target] (iterations)
 *
 * @param argc Argument count.
 * @param argv Argument value.
 * @return 0 if successful, 1 for errors.
 */
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [target] (iterations)\n";
    return 1;
  }

  // Check target existence and assign its name.
  ARG_EXIST_CHECK(Target, argv[1]);

  // Set maximum iterations or use default.
  int MaxIter = INT_MAX;
  if (argc == 3) {
    MaxIter = atoi(argv[2]);
  }

  struct stat Buffer;
  int Iter = 0;

  while (Iter < MaxIter) {
    std::cout << "\nIteration " << Iter << ": Running target program\n";

    // Execute target program.
    int Ret = std::system(Target.c_str());
    if (Ret) {
      std::cout << "Crashing input found after " << Iter << " iterations\n";
      break;
    }

    // Check if FormulaFile exists.
    if (stat(FormulaFile, &Buffer)) {
      std::cerr << FormulaFile << " not found\n";
      return 1;
    }

    // Generate new inputs based on FormulaFile.
    generateInput();
    Iter++;
  }

  return 0;
}
