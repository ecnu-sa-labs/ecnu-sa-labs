#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include "llvm/IR/IntrinsicInst.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <stack>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "Executor.h"
#include "ExecutionState.h"
#include "TerminationTypes.h"


using namespace llvm;
using namespace miniklee;

Executor::Executor(std::unique_ptr<llvm::Module> module) 
    : module(std::move(module)) {
    m_numTotalTests = 0;
    this->solver = createCoreSolver(CoreSolverType::TINY_SOLVER);
}

void Executor::runFunctionAsMain(Function *function) {
    ExecutionState initialState(function);
    states.addState(&initialState);

    // main interpreter loop
    while (!states.isEmpty()) {
        // REVIEW: Need searcher to choose next state?
        ExecutionState &state = states.selectState();

        Instruction *i = &*state.pc;
        stepInstruction(state);

        executeInstruction(state, i);

        updateStates(&state);
    }
}

void Executor::stepInstruction(ExecutionState& state) {
    state.prevPC = state.pc;
    ++state.pc;
}

void Executor::executeInstruction(ExecutionState& state, Instruction* i) {
    switch (i->getOpcode()) {
    // Control flow
    case Instruction::Ret: {
        errs() << "State " << state.getID() << " Ret\n";
        terminateState(state, StateTerminationType::Normal);
        break;
    }
    case Instruction::Br: {
        errs() << "State " << state.getID() << " Br\n";
        BranchInst *bi = cast<BranchInst>(i);
         if (bi->isUnconditional()) {
            transferToBasicBlock(bi->getSuccessor(0), state);
        } else {
            // Task 3: Your Code Here
            // Interpreting the semantics of Br
            // Please remove the blind transfer statement before implementing the semantics
            transferToBasicBlock(bi->getSuccessor(0), state);
        }
        break;
    }
    case Instruction::Call: {
        if (isa<DbgInfoIntrinsic>(i))
            break;

        const CallBase *cb = cast<CallBase>(i);
        if (needsSymbolization(i)) {
            assert(cb->arg_size()  == 3 && "Unexpected Error");
            processMakeSymbolic(state, cb);
            break;
        }

        const Function *f = cb->getCalledFunction();
        if (f->isIntrinsic() && f->getIntrinsicID() == Intrinsic::trap) {
            terminateState(state, StateTerminationType::Error);
        } else {
            assert(false && "Unsupported instruction");
        }
        break;
    }

    // Memory instructions...
    case Instruction::Alloca: {
        errs() << "State " << state.getID() << ": Alloca\n";
        AllocaInst *ai = cast<AllocaInst>(i);
        assert("Support Int32 type only" 
                && ai->getAllocatedType() == Type::getInt32Ty(i->getContext()));
        const DataLayout &dataLayout = module->getDataLayout();
        unsigned sizeInBits = dataLayout.getTypeSizeInBits(ai->getAllocatedType());
        executeAlloc(state, sizeInBits, i);
        break;
    }

    case Instruction::Load: {
        errs() << "State " << state.getID() << " Load\n";
        LoadInst *li = cast<LoadInst>(i);
        
        Instruction *address = dyn_cast<Instruction>(li->getPointerOperand());
        assert(address && "Pointer Operand expected");

        executeMemoryOperation(state, false, address, 0, li /* simply the Load instr itself */);
        break;
    }

    case Instruction::Store: {
        errs() << "State " << state.getID() << " Store\n";
        StoreInst *si = cast<StoreInst>(i);
        // Retrieve the target address
        Instruction *target = dyn_cast<Instruction>(si->getOperand(1));
        if (!target)
            assert(false && "Target is not an Instruction");
        // Retrieve the value to be stored
        Value *value = si->getValueOperand();

        if (ConstantInt *ci = dyn_cast<ConstantInt>(value)) {
            // Deal with constant, just assign value
            assert(ci->getType()->isIntegerTy(32) && "Int32 expected");
            int32_t rawInt32Value = static_cast<int32_t>(ci->getSExtValue());
            ref<miniklee::ConstantExpr> int32Value = miniklee::ConstantExpr::alloc(rawInt32Value, Expr::Int32);
            executeMemoryOperation(state, true, target, int32Value, 0);
        } else {
            // Deal with pointer, then fetch pointed and assign
            Instruction *ptr = dyn_cast<Instruction>(value); assert(ptr);
            ref<Expr> pointed = getInstructionValue(state, ptr);

            assert(pointed && "No coresponding symblic value found for pointer!");
            executeMemoryOperation(state, true, target, pointed, 0);
        }

        break;
    }

    // Arithmetic
    case Instruction::Add: {
        errs() << "State " << state.getID() << " Add\n";
        // Task 2: Your Code Here
        // Interpreting the semantics of Add
        // Please remove the assertion below after implementing the semantics
        assert(false && "Not Implemented yet");
        break;
    }

    case Instruction::Sub: {
        errs() << "State " << state.getID() << " Sub\n";
        // Task 2: Your Code Here
        // Interpreting the semantics of Sub
        // Please remove the assertion below after implementing the semantics
        assert(false && "Not Implemented yet");
        break;
    }

    // Compare
    case Instruction::ICmp: {
        CmpInst *ci = cast<CmpInst>(i);
        ICmpInst *ii = cast<ICmpInst>(ci);

        switch(ii->getPredicate()) {
        case ICmpInst::ICMP_EQ: {
            errs() << "State " << state.getID() << " ICMP_EQ comparison\n";
            ref<Expr> lshValue = getValue(state, ii->getOperand(0));
            ref<Expr> rshValue = getValue(state, ii->getOperand(1));
            ref<Expr> eq = EqExpr::create(lshValue, rshValue);

            executeMemoryOperation(state, true, ii, eq, 0);
            break;
        }
        case ICmpInst::ICMP_NE: {
            assert(false && "REVIEW: ICMP_NE comparison");
            break;
        }
        case ICmpInst::ICMP_UGT: {
            assert(false && "REVIEW: ICMP_UGT comparison");
            break;
        }
        case ICmpInst::ICMP_UGE: {
            assert(false && "REVIEW: ICMP_UGE comparison");
            break;
        }
        case ICmpInst::ICMP_ULT: {
            assert(false && "REVIEW: ICMP_ULT comparison");
            break;
        }
        case ICmpInst::ICMP_ULE: {
            assert(false && "REVIEW: ICMP_ULE comparison");
            break;
        }
        case ICmpInst::ICMP_SGT: {
            assert(false && "REVIEW: ICMP_SGT comparison");
            break;
        }
        case ICmpInst::ICMP_SGE: {
            assert(false && "REVIEW: ICMP_SGE comparison");
            break;
        }
        case ICmpInst::ICMP_SLT: {
            errs() << "State " << state.getID() << " ICMP_SLT comparison\n";
            ref<Expr> lshValue = getValue(state, ii->getOperand(0));
            ref<Expr> rshValue = getValue(state, ii->getOperand(1));
            ref<Expr> slt = SltExpr::create(lshValue, rshValue);

            executeMemoryOperation(state, true, ii, slt, 0);
            break;
        }
        case ICmpInst::ICMP_SLE: {
            assert(false && "REVIEW: ICMP_SLE comparison");
            break;
        }
        default:
            assert(false && " Unknown comparison. REVIEW: Use terminateStateOnExecError to finish.");
        }
        break;
    }
    
    default:
        errs() << "Unknown instruction: " << *i << "\n";
        assert(false && "Unknown instruction");
        break;
    }

}

void Executor::updateStates(ExecutionState *current)  {
    assert(current);

    states.addState(addedStates.begin(), addedStates.end());
    addedStates.clear();

    for (std::vector<ExecutionState *>::iterator it = removedStates.begin(),
                                                ie = removedStates.end();
        it != ie; ++it) {
        ExecutionState *es = *it;
        std::deque<ExecutionState*>::iterator it2 = states.find(es);
        assert(it2 != states.end());

        states.erase(it2);
    }
    removedStates.clear();
}

void Executor::transferToBasicBlock(BasicBlock *dst, ExecutionState &state) {
    state.pc = dst->begin();
}


void Executor::executeAlloc(ExecutionState& state, unsigned size, 
                            Instruction* inst) {
    assert("Only Support Int32"
        && size == Type::getInt32Ty(inst->getContext())->getPrimitiveSizeInBits());
    // REVIEW: Maybe we should refactor the ConstantExpr to be more specific
    state.locals.insert({inst, miniklee::InvalidKindExpr::create(0, size)});
}

ref<Expr> Executor::getInstructionValue(ExecutionState& state, Instruction* i) {
    auto it = state.locals.find(i);
    if (it != state.locals.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}


void Executor::executeMemoryOperation(ExecutionState& state, 
                                    bool isWrite, 
                                    Instruction *address,
                                    ref<Expr> value, /* undef if read */
                                    Instruction* target /* undef if wirte*/) {
    if (isWrite) { // Interpret the Store instruction
        assert(!target);
        auto it = state.locals.find(address);
        if (it != state.locals.end()) {
            state.locals.erase(it);
        }
        state.locals.insert({address, value});
    } else { // Interpret the Load instruction
        assert(!value);
        auto loadedValue = state.locals.find(address);
        if (loadedValue == state.locals.end()) {
            assert(false && "Unexpected Error");
        } else {
            auto it = state.locals.find(target);
            if (it != state.locals.end()) {
                state.locals.erase(it);
            }
            state.locals.insert({target, loadedValue->second});
        }
    }
}

bool Executor::needsSymbolization(Instruction *i) {
    const CallBase *cb = cast<CallBase>(i);
    if (cb->getCalledFunction()->getName() == "make_symbolic")
        return true;
    else
        return false;
}

void Executor::processMakeSymbolic(ExecutionState &state, const CallBase *cb) {
    errs() << "State " << state.getID() << " Mk Sym\n";
    // 1. Make symbolic
    Instruction *sym = dyn_cast<Instruction>(cb->getArgOperand(0));
    assert(sym && "First argument should be a variable (Instruction type)");

    // 2. Deal with the size (4 bytes)
    ConstantInt *size = dyn_cast<ConstantInt>(cb->getArgOperand(1));
    assert(size->getSExtValue() == Expr::Int32 / CHAR_BIT);

    // 3. Retrieve the name
    Value *arg = cb->getArgOperand(2); 
    // GEP constant expression
    auto *gepExpr = cast<llvm::ConstantExpr>(arg);
    assert(gepExpr->getOpcode() == llvm::Instruction::GetElementPtr && "GEP Expected");
    // Get the global variable
    auto *globalVar = cast<GlobalVariable>(gepExpr->getOperand(0));
    auto *strArray = cast<ConstantDataArray>(globalVar->getInitializer());
    assert(strArray->isString() && "String Expected");

    executeMakeSymbolic(state, sym, strArray->getAsString().str());
}

ref<Expr> Executor::getValue(ExecutionState& state, Value* value) {
    if (Instruction *v= dyn_cast<Instruction>(value)) {
        ref<Expr> rawValue = getInstructionValue(state, v);
        assert(rawValue && "Value Not Stored");
        return rawValue;
    } else if (ConstantInt *constValue = dyn_cast<ConstantInt>(value)) {
        return miniklee::ConstantExpr::create(
            static_cast<int32_t>(constValue->getSExtValue()),
            Expr::Int32);
    } else {
        assert(false && "Unexpected Error");
    }

}

void Executor::executeMakeSymbolic(ExecutionState& state,
                                   Instruction *symAddress,
                                   std::string name) {
    // Task 1: Your Code Here.
    // Implementing the Process of Symbolization.
    // Please remove the assertion below after implementing the semantics.
    assert(false && "Not Implemented yet");
}


Executor::StatePair Executor::fork(ExecutionState &current,
                                    ref<Expr> condition) {
    // Task 3: Your Code Here.
    // Interpreting the semantics of Br.
    // Please remove the assertion below after implementing the semantics.
    assert(false && "Not Implemented yet");
}

void Executor::addConstraint(ExecutionState &state, ref<Expr> condition) {
    if (miniklee::ConstantExpr *CE = 
            dyn_cast<miniklee::ConstantExpr>(condition.get())) {
        if (!CE->isTrue())
        llvm::report_fatal_error("attempt to add invalid constraint");
        return;
    }

    state.addConstraint(condition);
}

void Executor::terminateState(ExecutionState &state,
                              StateTerminationType reason) {
    // Generate test case
    processTestCase(state, reason);
    // Terminate
    std::vector<ExecutionState *>::iterator it =
      std::find(addedStates.begin(), addedStates.end(), &state);
    if (it == addedStates.end()) {
        state.pc = state.prevPC;
        removedStates.push_back(&state);
    } else {
        // never reached, just delete immediately
        addedStates.erase(it);
        delete &state;
    }
}

inline bool directoryExists(const std::string &path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

inline bool createDirectory(const std::string &path) {
    return mkdir(path.c_str(), 0755) == 0;
}

inline std::string stateTy2Str(StateTerminationType type) {
    switch (type) {
        case StateTerminationType::Error: return "error";
        case StateTerminationType::Normal: return "normal";
        default: return "unknown";
    }
}

void Executor::processTestCase(const ExecutionState &state,
                               StateTerminationType type) {
    std::vector< std::pair<std::string, int32_t> > out;
    bool success = getSymbolicSolution(state, out);
    if (!success)
        llvm::report_fatal_error("Could not compute solution.");
    
    if (success) {
        // Create the dir
        static std::string dir;
        if (dir.empty()) {
            unsigned dir_id = 1;
            do {
                dir = "result_" + std::to_string(dir_id);
                dir_id++;
            } while (directoryExists(dir));
            if (!createDirectory(dir)) {
                std::cerr << "Error: Failed to create dir " << dir << std::endl;
                return;
            }
        }

        unsigned id = ++m_numTotalTests;
        std::string filePath = dir 
                            + "/test_case_" 
                            + std::to_string(id) 
                            + "." + stateTy2Str(type);

        // Write test cases to the file
        std::ofstream file(filePath, std::ios::out | std::ios::trunc);
        for (const auto &pair : out) {
            file << pair.first.c_str() << ", " << pair.second << "\n";
        }
        file.close();

        // Output success message
        std::cout << GREEN_TEXT("Test case written successfully (" + filePath + ")") << std::endl;
    }
}

bool Executor::getSymbolicSolution(const ExecutionState &state, 
                                  std::vector<std::pair<std::string, int32_t>> &res) {
    std::vector<int32_t> values;
    std::vector<const SymbolicExpr *> objects;

    for (auto &kv : state.locals) {
        if (SymbolicExpr *sym = dyn_cast<SymbolicExpr>(kv.second.get())) {
            bool notFound = find(objects.begin(), objects.end(), sym) == objects.end();
            if (notFound)
                objects.push_back(sym);
        }
    }

    bool success = solver->getInitialValues(
        Query(state.constraints, miniklee::ConstantExpr::create(1, Expr::Int32)),
        objects, values);
    
    for (unsigned i = 0; i != objects.size(); ++i) {
        std::string name = objects[i]->getName();
        res.push_back(std::make_pair(name, values[i]));
    }

    return success;
}
