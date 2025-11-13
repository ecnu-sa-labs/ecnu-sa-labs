#include "Solver.h"

#include "Constraints.h"
#include "SolverImpl.h"

#include <utility>

using namespace miniklee;
using namespace llvm;

const char *Solver::validity_to_str(Validity v) {
    switch (v) {
    default:    return "Unknown";
    case True:  return "True";
    case False: return "False";
    }
}

Solver::Solver(std::unique_ptr<SolverImpl> impl) : impl(std::move(impl)) {}
Solver::~Solver() = default;

std::string Solver::getConstraintLog(const Query& query) {
    return impl->getConstraintLog(query);
}

void Solver::setCoreSolverTimeout(time::Span timeout) {
    impl->setCoreSolverTimeout(timeout);
}

bool Solver::evaluate(const Query& query) {
    // Maintain invariants implementations expect.
    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(query.expr.get())) {
        if (CE->isTrue())
            return true;
        else
            return false;
    }
    return impl->computeValidity(query);
}

bool Solver::mustBeTrue(const Query& query, bool &result) {
    assert(query.expr->getWidth() == Expr::Bool && "Invalid expression type!");

    // Maintain invariants implementations expect.
    if (ConstantExpr *CE = dyn_cast<ConstantExpr>(query.expr.get())) {
        result = CE->isTrue() ? true : false;
        return true;
    }

    return impl->computeTruth(query, result);
}

bool Solver::mustBeFalse(const Query& query, bool &result) {
    return mustBeTrue(query.negateExpr(), result);
}

bool Solver::mayBeTrue(const Query& query, bool &result) {
    bool res;
    if (!mustBeFalse(query, res))
        return false;
    result = !res;
    return true;
}

bool Solver::mayBeFalse(const Query& query, bool &result) {
    bool res;
    if (!mustBeTrue(query, res))
        return false;
    result = !res;
    return true;
}

bool Solver::getValue(const Query& query, ref<ConstantExpr> &result) {
    assert(false && "Not implemented yet");
}

bool 
Solver::getInitialValues(const Query& query,
                            const std::vector<const SymbolicExpr*> &objects,
                            std::vector<int32_t > &values) {
    bool success =
        impl->computeInitialValues(query, objects, values);
    return success;
}

std::pair< ref<Expr>, ref<Expr> > Solver::getRange(const Query& query) {
    assert(false && "Not implemented yet");
}
