/**
 * @file ChaoticIteration.cpp
 * @brief Chaotic Iteration Algorithm.
 */

#include "DivZeroAnalysis.h"
#include "Utils.h"

namespace dataflow {

/**
 * @brief Get predecessors of a given instruction in control-flow graph.
 *
 * @param Inst Instruction for which to get predecessors.
 * @return Vector of all predecessors of Inst.
 */
std::vector<Instruction *> getPredecessors(Instruction *Inst) {
  std::vector<Instruction *> Ret;
  auto Block = Inst->getParent();

  // Iterate through instructions in block in reverse order to find given
  // instruction.
  for (auto Iter = Block->rbegin(), End = Block->rend(); Iter != End; ++Iter) {
    if (&(*Iter) == Inst) {
      ++Iter;

      // If previous instruction exists in same block, add it to predecessors.
      if (Iter != End) {
        Ret.push_back(&(*Iter));
        return Ret;
      }

      // If instruction is first in its block, get last instruction from each
      // predecessor block.
      for (auto Pre = pred_begin(Block), BE = pred_end(Block); Pre != BE;
           ++Pre) {
        Ret.push_back(&(*((*Pre)->rbegin())));
      }
      return Ret;
    }
  }
  return Ret;
}

/**
 * @brief Get successors of a given instruction in control-flow graph.
 *
 * @param Inst Instruction for which to get successors.
 * @return Vector of all successors of Inst.
 */
std::vector<Instruction *> getSuccessors(Instruction *Inst) {
  std::vector<Instruction *> Ret;
  auto Block = Inst->getParent();

  // Iterate through instructions in block in reverse order to find given
  // instruction.
  for (auto Iter = Block->begin(), End = Block->end(); Iter != End; ++Iter) {
    if (&(*Iter) == Inst) {
      ++Iter;

      // If next instruction exists in same block, add it to successors.
      if (Iter != End) {
        Ret.push_back(&(*Iter));
        return Ret;
      }

      // If instruction is last in its block, get first instruction from each
      // successor block.
      for (auto Succ = succ_begin(Block), BS = succ_end(Block); Succ != BS;
           ++Succ) {
        Ret.push_back(&(*((*Succ)->begin())));
      }
      return Ret;
    }
  }
  return Ret;
}

/**
 * @brief Joins two Memory objects (Mem1 and Mem2), accounting for Domain
 * values.
 *
 * @param Mem1 1st memory.
 * @param Mem2 2nd memory.
 * @return Joined memory.
 */
Memory *join(Memory *Mem1, Memory *Mem2) {
  /**
   * TODO: Write your code that joins two memories.
   *
   * If some instruction with domain D is either in Mem1 or Mem2, but not in
   *   both, add it with domain D to Result.
   * If some instruction is present in Mem1 with domain D1 and in Mem2 with
   *   domain D2, then Domain::join D1 and D2 to find new domain D,
   *   and add instruction I with domain D to Result.
   */
  return NULL;
}

/**
 * @brief Computes input memory state for a given instruction.
 *
 * @param Inst Instruction for which to compute input memory state.
 * @param InMem Memory state that will be populated with joined state of
 * instruction's predecessors.
 */
void DivZeroAnalysis::flowIn(Instruction *Inst, Memory *InMem) {
  /**
   * TODO: Write your code to implement flowIn.
   *
   * For each predecessor Pred of instruction Inst, do following:
   *   + Get Out Memory of Pred using OutMap.
   *   + Join Out Memory with InMem.
   */
}

/**
 * @brief Check if two memories (Mem1 and Mem2) are equal.
 *
 * @param Mem1 1st memory
 * @param Mem2 2nd memory
 * @return true if two memories are equal, false otherwise.
 */
bool equal(Memory *Mem1, Memory *Mem2) {

  /**
   * TODO: Write your code to implement check for equality of two memories.
   *
   * If any instruction I is present in one of Mem1 or Mem2,
   *   but not in both and Domain of I is not UnInit, memories are
   *   unequal.
   * If any instruction I is present in Mem1 with domain D1 and in Mem2
   *   with domain D2, if D1 and D2 are unequal, then memories are unequal.
   */
  return false;
}

/**
 * @brief Computes output memory state for a given instruction.
 *
 * @param Inst Instruction for which to compute output memory state.
 * @param Pre Memory state before transfer function is applied.
 * @param Post Memory state after transfer function is applied.
 * @param WorkSet Set of instructions that need to be re-analyzed due to changes
 * in their input state.
 */
void DivZeroAnalysis::flowOut(Instruction *Inst, Memory *Pre, Memory *Post,
                              SetVector<Instruction *> &WorkSet) {
  /**
   * TODO: Write your code to implement flowOut.
   *
   * For each given instruction, merge abstract domain from pre-transfer memory
   * and post-transfer memory, and update OutMap.
   * If OutMap changed then also update? WorkSet.
   */
}

/**
 * @brief Main function for ChaoticIteration.cpp.
 *
 * @param F Function on which to perform dataflow analysis.
 */
void DivZeroAnalysis::doAnalysis(Function &F) {
  SetVector<Instruction *> WorkSet;
  /**
   * TODO: Write your code to implement chaotic iteration algorithm
   * for analysis.
   *
   * Initialize WorkSet with all instructions in function.
   *
   * While WorkSet is not empty:
   * (1) Pop an instruction from WorkSet. (2) Construct it's Incoming Memory
   * using flowIn. (3) Evaluate instruction using transfer and create OutMemory.
   * Call transfer create map and pass reference to transfer, make a copy of
   * inmap and pass in , because you don't want to change.
   * - Use flowOut along with previous Out memory and current Out
   *   memory, to check if there is a difference between two to update
   *   OutMap and add all successors to WorkSet.
   */
}

} // namespace dataflow