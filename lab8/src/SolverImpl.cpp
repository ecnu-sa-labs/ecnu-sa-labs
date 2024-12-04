#include "Solver.h"
#include "SolverImpl.h"

using namespace miniklee;

SolverImpl::~SolverImpl() {}

bool SolverImpl::computeValidity(const Query &query) {
    assert(false && "TBD");
}

const char *SolverImpl::getOperationStatusString(SolverRunStatus statusCode) {
    switch (statusCode) {
    case SOLVER_RUN_STATUS_SUCCESS_SOLVABLE:
        return "OPERATION SUCCESSFUL, QUERY IS SOLVABLE";
    case SOLVER_RUN_STATUS_SUCCESS_UNSOLVABLE:
        return "OPERATION SUCCESSFUL, QUERY IS UNSOLVABLE";
    case SOLVER_RUN_STATUS_FAILURE:
        return "OPERATION FAILED";
    case SOLVER_RUN_STATUS_TIMEOUT:
        return "SOLVER TIMEOUT";
    case SOLVER_RUN_STATUS_FORK_FAILED:
        return "FORK FAILED";
    case SOLVER_RUN_STATUS_INTERRUPTED:
        return "SOLVER PROCESS INTERRUPTED";
    case SOLVER_RUN_STATUS_UNEXPECTED_EXIT_CODE:
        return "UNEXPECTED SOLVER PROCESS EXIT CODE";
    case SOLVER_RUN_STATUS_WAITPID_FAILED:
        return "WAITPID FAILED FOR SOLVER PROCESS";
    default:
        return "UNRECOGNIZED OPERATION STATUS";
    }
}
