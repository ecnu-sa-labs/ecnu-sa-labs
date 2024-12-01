#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stack>
#include <iostream>
#include "ExecutionState.h"
#include "Solver.h"
#include "TerminationTypes.h"

using namespace llvm;

// Color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREY    "\033[1;30m"

// Macros for colored text
#define GREEN_TEXT(text) COLOR_GREEN text COLOR_RESET
#define BLUE_TEXT(text)  COLOR_BLUE text COLOR_RESET
#define RED_TEXT(text)   COLOR_RED text COLOR_RESET
#define GREY_TEXT(text)  COLOR_GREY text COLOR_RESET

class StateManager {
public:
    void addState(ExecutionState* state) {
        states.push_back(state);
    }

    template <typename Iterator>
    void addState(Iterator begin, Iterator end) {
        states.insert(states.end(), begin, end);
    }

    ExecutionState& selectState() {
        assert(!states.empty());
        return *states.front();
    }

    bool isEmpty() const { return states.empty(); }

    std::deque<ExecutionState*>::iterator find(ExecutionState* target) {
        return std::find(states.begin(), states.end(), target);
    }

    std::deque<ExecutionState*>::iterator begin() { return states.begin(); }

    std::deque<ExecutionState*>::iterator end() { return states.end(); }

    std::deque<ExecutionState*>::iterator erase(std::deque<ExecutionState*>::iterator pos) {
        return states.erase(pos);
    }

private:
    std::deque<ExecutionState*> states;
};

class Executor {
public:
    /**
     * A unique pointer to an LLVM module.
     * 
     * Represents the current LLVM IR module being processed or analyzed,
     * ensuring exclusive ownership and automatic cleanup.
     */
    std::unique_ptr<llvm::Module> module;
    
    std::unique_ptr<Solver> solver;

    /**
     * The StateManager class manages a collection of execution states.
     * 
     * It provides functionality to add, select, check if the collection is
     * empty, find a specific state, and erase states.
     */
    StateManager states;

    typedef std::pair<ExecutionState*,ExecutionState*> StatePair;

    void runFunctionAsMain(llvm::Function* function);

    // Constructor that accepts an llvm::Module pointer
    explicit Executor(std::unique_ptr<llvm::Module> module);

    // Used to track states that have been added during the current
    // instructions step. 
    std::vector<ExecutionState *> addedStates;
    // Used to track states that have been removed during the current
    // instructions step. 
    std::vector<ExecutionState *> removedStates;

    // Number of tests received from the interpreter
    unsigned m_numTotalTests;
private:

    void stepInstruction(ExecutionState& state);
    void executeInstruction(ExecutionState& state, llvm::Instruction* inst);
    void updateStates(ExecutionState *current);

    void transferToBasicBlock(llvm::BasicBlock *dst, ExecutionState &state);

    /**
     * Handles allocation within the virtual execution environment.
     * 
     * Processes allocation instructions, ensures the size matches the 
     * system's expectations, and updates the local state with the result.
     * 
     * @param state The current execution state with local variables.
     * @param size The allocation size in bits.
     * @param inst A pointer to the allocation instruction.
     * 
     * Note: Only 32-bit allocations are currently supported. Using 
     * ConstantExpr for allocation results may need revisiting in future.
     */
    void executeAlloc(ExecutionState& state, unsigned size, llvm::Instruction* inst);

    ref<Expr> getInstructionValue(ExecutionState& state, llvm::Instruction* i);

    /**
     * Executes memory operations (load/store) for the current state.
     *
     * @param state The current execution state with local variables.
     * @param isWrite True for a write (store), false for a read (load).
     * @param address The variable's memory address.
     * @param value The value to write (only for store operations).
     * @param target The destination for the read value (only for load operations).
     */
    void executeMemoryOperation(ExecutionState& state,
                                bool isWrite,
                                llvm::Instruction *address,
                                ref<Expr> value,
                                llvm::Instruction* i);

    /**
     * Retrieves the expression value of a given value in the current execution state.
     *
     * This function converts an LLVM IR value into an `Expr` for further 
     * processing and analysis. It determines the conversion method by 
     * examining the type of the provided `Value` pointer.
     *
     * @param state The current execution state containing all information 
     * up to this point.
     * @param value A pointer to the LLVM IR value to be converted into an `Expr`.
     * @return A reference to an `Expr` representing the converted value.
     */
    ref<Expr> getValue(ExecutionState& state,
                       Value* value /* ConstantInt* or Instruction* */);
        
    void executeMakeSymbolic(ExecutionState& state,
                             Instruction *sym, std::string name);

    /* 
    * The fork function handles branching of the execution state when 
    * encountering a conditional statement. It evaluates the condition 
    * under the current constraints to determine how to branch.
    * 
    * @param current The current execution state.
    * @param condition The condition expression for the branch.
    * @return StatePair Contains pointers to the execution states after 
    * branching, which may be (trueBranch, falseBranch).
    */
    StatePair fork(ExecutionState &current, ref<Expr> condition);

    // Add the given condition as a constraint on state. This
    // function is a wrapper around the state's addConstraint function.
    void addConstraint(ExecutionState &state, ref<Expr> condition);

    void terminateState(ExecutionState &state, StateTerminationType reason);
    
    void processTestCase(const ExecutionState &state, StateTerminationType type);
    bool getSymbolicSolution(const ExecutionState &state, 
            std::vector<std::pair<std::string, int32_t>> &res);
    
    bool needsSymbolization(Instruction *i);
    void processMakeSymbolic(ExecutionState &state, const CallBase *cb);
};


#endif // EXECUTOR_H