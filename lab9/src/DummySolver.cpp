#include "Solver.h"
#include "SolverImpl.h"

#include <memory>

namespace miniklee {

class DummySolverImpl : public SolverImpl {
public:
    DummySolverImpl();

    bool computeValidity(const Query &);
    bool computeTruth(const Query &, bool &isValid);
    bool computeValue(const Query &, ref<Expr> &result);
    bool computeInitialValues(const Query &,
                                const std::vector<const SymbolicExpr *> &objects,
                                std::vector<int32_t> &values);
    SolverRunStatus getOperationStatusCode();
};

DummySolverImpl::DummySolverImpl() {}

bool DummySolverImpl::computeValidity(const Query &) {
    return false;
}

bool DummySolverImpl::computeTruth(const Query &, bool &isValid) {
    return false;
}

bool DummySolverImpl::computeValue(const Query &, ref<Expr> &result) {
    return false;
}

bool DummySolverImpl::computeInitialValues(
    const Query &, const std::vector<const SymbolicExpr *> &objects,
    std::vector<int32_t> &values) {
    return false;
}

SolverImpl::SolverRunStatus DummySolverImpl::getOperationStatusCode() {
    return SOLVER_RUN_STATUS_FAILURE;
}

std::unique_ptr<Solver> createDummySolver() {
    return std::make_unique<Solver>(std::make_unique<DummySolverImpl>());
}
}
