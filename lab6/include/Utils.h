#ifndef UTILS_H
#define UTILS_H

#include "DivZeroAnalysis.h"

extern const char *WHITESPACES;

namespace dataflow {

/**
 * @brief Get a human-readable string name for an llvm Value
 *
 * @param Val The llvm Value to get the string representation of
 * @return std::string The string representation of Val.
 */
std::string variable(const Value *Val);

/**
 * @brief Encode the memory address of an llvm Value
 *
 * @param Val The llvm Value to get the encoding of
 * @return std::string The encoded memory address of Val
 */
std::string address(const Value *Val);

/**
 * @brief Try to extract the Domain of a value.
 *
 * When Val is a ConstantInt, we can extract its Domain.
 *
 * @param Val Value to extract the Domain for.
 * @return Domain::Element Domain of Val, or Domain::Uninit if
 *  Val is not a ConstantInt.
 */
Domain::Element extractFromValue(const Value *Val);

/**
 * @brief Get the Domain of Val from Memory Or try Extracting it.
 *
 * @param Mem Memory containing the domain of Val.
 * @param Val Value whose domain is to be extracted from Mem.
 * @return Domain* Domain of Val in Mem
 */
Domain *getOrExtract(const Memory *Mem, const Value *Val);

/**
 * @brief Print the Memorm Mem in a human readable format to stderr.
 *
 * Format:
 *   [ <variable1> |-> <domain1> ]
 *   [ <variable2> |-> <domain2> ]
 *  ...
 *
 * @param Mem Memory to print
 */
void printMemory(const Memory *Mem);

/**
 * @brief Print the Before and After domains of an instruction
 * wrt. In and Out memory.
 *
 * Format:
 *   <instruction>:    [ <before> --> <after> ]
 *
 * @param Inst The instruction to print the domains for.
 * @param InMem The incoming memory.
 * @param OutMem The outgoing memory.
 */
void printInstructionTransfer(Instruction *Inst, const Memory *InMem,
                              const Memory *OutMem);

/**
 * @brief Print the In and Out memory of every instruction in function F to
 * stderr.
 *
 * This gives the human-readable representaion of the results of dataflow
 * analysis.
 *
 * @param F Function whose dataflow analysis result to print.
 * @param InMap Map of In memory of every instruction in function F.
 * @param OutMap Map of Out memory of every instruction in function F.
 */
void printMap(Function &F, ValueMap<Instruction *, Memory *> &InMap,
              ValueMap<Instruction *, Memory *> &OutMap);

} // namespace dataflow

#endif // UTILS_H