// #include "Z3Solver.h"

#include "Solver.h"

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <memory>

namespace miniklee {

std::unique_ptr<Solver> createCoreSolver(CoreSolverType cst) {
    switch (cst) {
    case DUMMY_SOLVER:
        return createDummySolver();
    case TINY_SOLVER:
        return createTinySolver();
    case Z3_SOLVER:
    #ifdef ENABLE_Z3
        klee_message("Using Z3 solver backend");
        return std::make_unique<Z3Solver>();
    #else
        llvm::errs() << "Not compiled with Z3 support";
        return NULL;
    #endif
    case NO_SOLVER:
        llvm::errs() << "Invalid solver";
        return NULL;
    default:
        llvm_unreachable("Unsupported CoreSolverType");
    }
}
}
