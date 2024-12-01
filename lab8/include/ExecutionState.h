#ifndef EXECUTIONSTATE_H
#define EXECUTIONSTATE_H

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>
#include <string>

#include "Expr.h"
#include "Constraints.h"

using namespace miniklee;

class ExecutionState {
public:
    // When encountering a Branch instruction, check the condition and jump to the correct instruction
    // Pointer to instruction to be executed after the current instruction
    llvm::BasicBlock::iterator pc;

    // Pointer to instruction which is currently executed
    llvm::BasicBlock::iterator prevPC;

    // Store symbolic variables and their values
    std::unordered_map<const llvm::Value*, ref<Expr>> locals;

    // Path constraints collected so far
    ConstraintSet constraints;

    // The global state counter
    static std::uint32_t nextID;

    // the state id
    std::uint32_t id = 0;

public:
    ExecutionState() {}

    // Only to create the initial state
    ExecutionState(llvm::Function *f);

    // Copy constructor
    ExecutionState(const ExecutionState& state);

    ExecutionState *branch();

    void addConstraint(ref<Expr> e);

    // Get the unique identifier (ID) of the execution state.
    std::uint32_t getID() const { return id; };
    void setID() { id = nextID++; };
};

#endif // EXECUTIONSTATE_H
