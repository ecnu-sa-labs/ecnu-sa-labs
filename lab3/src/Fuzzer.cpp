/**
 * @file Fuzzer.cpp
 * @brief Main driver for Fuzzer.
 */

/**
 * NOTE: You are free to modify any content in this .cpp file. This means you
 * can change almost everything, as long as fuzzer runs with same CLI interface.
 * If you're satisfied with some of provided default implementations, you don't
 * need to modify them.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "Utils.h"

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

/**
 * @def DBG
 * @brief Debug macro to output current filename and line number.
 */
#define DBG                                                                    \
  std::cout << "Hit F::" << __FILE__ << " ::L" << __LINE__ << std::endl

/**
 * @typedef MutationFn
 * @brief Type Signature of Mutation Function.
 */
typedef std::string MutationFn(std::string);

/**
 * @struct RunInfo
 * @brief Holds useful information about one run of program.
 *
 * @param Passed       Did a program run without crashing?
 * @param Mutation     Mutation function used for this run.
 * @param Input        Parent input used for generating input for this run.
 * @param MutatedInput Input string for this run.
 */
struct RunInfo {
  bool Passed;
  MutationFn *Mutation;
  std::string Input, MutatedInput;
};

/************************************************/
/*            Global state variables            */
/************************************************/
/**
 * Note: Feel free to add/remove/change any of following variables.
 * Depending on what you want to keep track of during fuzzing.
 */

/**
 * @brief Collection of strings used to generate inputs.
 */
std::vector<std::string> SeedInputs;

/**
 * @brief Coverage related information from previous step.
 */
std::vector<std::string> PrevCoverageState;

/**
 * @brief Variable to store coverage related information.
 */
std::vector<std::string> CoverageState;

/**
 * @brief Variable to keep track of some Mutation related state.
 * Feel free to change/ignore this if you want to.
 */
int MutationState = 0;

/**
 * @brief Variable to keep track of some state related to strategy selection.
 * Feel free to change/ignore this if you want to.
 */
int StrategyState = 0;

/************************************************/
/*    Implement your select input algorithm.    */
/************************************************/

/**
 * @brief Select a string that will be mutated to generate a new input.
 * Sample code picks the first input string from SeedInputs.
 *
 * TODO: Implement your logic for selecting a input to mutate. If you require,
 * you can use Info variable to help make a decision while selecting a Seed but
 * it is not necessary for lab.
 *
 * @param RunInfo struct with information about previous run.
 * @return Pointer to a string.
 */
std::string selectInput(RunInfo Info) {
  int Index = 0;
  return SeedInputs[Index];
}

/*********************************************/
/*      Implement mutation startegies.       */
/*********************************************/

const char ALPHA[] = "abcdefghijklmnopqrstuvwxyz\n\0";
const int LENGTH_ALPHA = sizeof(ALPHA);

/**
 * Here we provide a two sample mutation functions that take as input a string
 * and returns a string.
 */

/**
 * @brief Mutation Strategy that does nothing.
 *
 * @param Original Input string to be mutated.
 * @return Mutated string.
 */
std::string mutation_0(std::string Original) { return Original; }

/**
 * @brief Mutation Strategy that inserts a random alpha numeric char at a random
 * location in Original. Such as: 'abc' -> 'abXc'.
 *
 * @param Original Input string to be mutated.
 * @return Mutated string.
 */
std::string mutation_1(std::string Original) {
  if (Original.length() <= 0)
    return Original;
  int Index = rand() % Original.length();
  return Original.insert(Index, 1, ALPHA[rand() % LENGTH_ALPHA]);
}

/**
 * TODO: Add your own mutation functions below. Make sure to update MutationFns
 * vector to include your functions.
 *
 * Some ideas: swap adjacent chars, increment/decrement a char.
 *
 * Get creative with your strategies.
 */

/**
 * @brief Vector containing all available mutation functions.
 *
 * TODO: Update definition to include any mutations you implement.
 * For example if you implement mutation_2 then update it to be:
 * std::vector<MutationFn *> MutationFns = {mutation_0, mutation_1, mutation_2};
 */
std::vector<MutationFn *> MutationFns = {mutation_0, mutation_1};

/**
 * @brief Select a mutation function to apply to seed input.
 * Sample code picks a random Strategy.
 *
 * TODO: Add your own logic to select a mutation function from MutationFns.
 * Hint: You may want to make use of any global state you store during feedback
 * to make decisions on what MutationFn to choose.
 *
 * @param RunInfo Struct with information about current run.
 * @returns A pointer to a MutationFn.
 */
MutationFn *selectMutationFn(RunInfo &Info) {
  int Strat = rand() % MutationFns.size();
  return MutationFns[Strat];
}

/*********************************************/
/*     Implement your feedback algorithm.    */
/*********************************************/
/**
 * Update internal state of fuzzer using coverage feedback.
 *
 * @param Target Name of target binary.
 * @param Info RunInfo.
 */
void feedBack(std::string &Target, RunInfo &Info) {
  // Store raw coverage data.
  std::vector<std::string> RawCoverageData;
  // Read coverage data from target binary into vector.
  readCoverageFile(Target, RawCoverageData);

  // Backup current coverage state.
  PrevCoverageState = CoverageState;
  // Clear current coverage state for new data.
  CoverageState.clear();

  /**
   * TODO: Implement your logic to use coverage information from test
   * phase to guide fuzzing. The sky is the limit!
   *
   * Hint: You want to rely on some amount of randomness to make decisions.
   *
   * You have Coverage information of previous test in PrevCoverageState. And
   * raw coverage data is loaded into RawCoverageData from Target.cov file.
   * You can either use this raw data directly or process it (not-necessary). If
   * you do some processing, make sure to update CoverageState to make it
   * available in the next call to feedback.
   */
  CoverageState.assign(RawCoverageData.begin(),
                       RawCoverageData.end()); // No extra processing.
}

int Freq = 1000;
int Count = 0;
int PassCount = 0;

/**
 * @brief Test target program with given input and handle coverage and crash
 * data.
 *
 * @param Target Path to the target program to be tested.
 * @param Input Input string to be fed to target program.
 * @param OutDir Dir where results (passing and crashing inputs) are stored.
 * @return true if target program ran without crashing, false otherwise.
 */
bool test(std::string &Target, std::string &Input, std::string &OutDir) {
  // Construct path for coverage data file.
  const std::string CoveragePath = Target + ".cov";
  // Remove any existing coverage data file before running target.
  std::remove(CoveragePath.c_str());

  // Increment count of tests run.
  ++Count;
  // Run target program with given input and capture its return code.
  const int ReturnCode = runTarget(Target, Input);

  // Check if target program was not found and exit if so.
  if (ReturnCode == 127) {
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }


  // If return code is non-zero, it indicates a crash.
  if (ReturnCode != 0) {
    // Store input that caused crash.
    storeCrashingInput(Input, OutDir);
    // Print number of inputs tried and number of crashes found so far.
    fprintf(stderr, "\e[A\rTried %d inputs, %d crashes found\n", Count,
      failureCount);
    return false;
  }

  // Print number of inputs tried and number of crashes found so far.
  fprintf(stderr, "\e[A\rTried %d inputs, %d crashes found\n", Count,
    failureCount);

  // Store passing inputs at a specified frequency.
  if (PassCount++ % Freq == 0) {
    storePassingInput(Input, OutDir);
  }

  return true;
}

/**
 * @brief Fuzz target program and store results to OutDir.
 *
 * @param Target Target (instrumented) program binary.
 * @param OutDir Dir to store fuzzing results.
 */
void fuzz(std::string Target, std::string OutDir) {
  struct RunInfo Info;
  while (true) {
    std::string Input = selectInput(Info);
    Info = RunInfo();
    Info.Input = Input;
    Info.Mutation = selectMutationFn(Info);
    Info.MutatedInput = Info.Mutation(Info.Input);
    Info.Passed = test(Target, Info.MutatedInput, OutDir);
    feedBack(Target, Info);
  }
}

/**
 * @brief Main function.
 * Usage:
 * ./fuzzer [target] [seed input dir] [output dir] [frequency] [random seed]
 *
 * @param argc Argument count.
 * @param argv Argument value.
 * @return 0 if successful, 1 for errors.
 */
int main(int argc, char **argv) {
  // Check for minimum required arguments.
  if (argc < 4) {
    printf("usage %s [target] [seed input dir] [output dir] [frequency "
           "(optional)] [seed (optional arg)]\n",
           argv[0]);
    return 1;
  }

  // Check for existence of target and directories.
  ARG_EXIST_CHECK(Target, argv[1]);
  ARG_EXIST_CHECK(SeedInputDir, argv[2]);
  ARG_EXIST_CHECK(OutDir, argv[3]);

  // Set frequency if provided.
  if (argc >= 5) {
    Freq = strtol(argv[4], NULL, 10);
  }

  // Set random seed for fuzzing.
  int RandomSeed = argc > 5 ? strtol(argv[5], NULL, 10) : (int)time(NULL);
  srand(RandomSeed);
  storeSeed(OutDir, RandomSeed);
  initialize(OutDir);

  // Read seed inputs.
  if (readSeedInputs(SeedInputs, SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }

  // Start fuzzing.
  fprintf(stderr, "Fuzzing %s...\n\n", Target.c_str());
  fuzz(Target, OutDir);

  return 0;
}
