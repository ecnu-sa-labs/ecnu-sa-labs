#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>
#include <string>

#include "ExecutionState.h"

std::uint32_t ExecutionState::nextID = 1;

ExecutionState::ExecutionState(llvm::Function* function)
    : pc(function->begin()->begin()), prevPC(nullptr) {
        setID();
}

ExecutionState::ExecutionState(const ExecutionState& state):
    pc(state.pc),
    prevPC(state.prevPC),
    locals(state.locals),
    constraints(state.constraints) {}

ExecutionState *ExecutionState::ExecutionState::branch() {
    auto *falseState = new ExecutionState(*this);
    falseState->setID();
    return falseState;
}

void ExecutionState::addConstraint(ref<Expr> e) {
    // REVIEW: Consider adding a constraint manager for optimization.
    constraints.push_back(e);
}
