#include "Solver.h"
#include "Constraints.h"
#include "SolverImpl.h"

#include <memory>
#include <random>
#include <algorithm>
#include <iostream>

namespace miniklee {

class TinySolverImpl : public SolverImpl {
public:
    TinySolverImpl();

    bool computeValidity(const Query &);
    bool computeTruth(const Query &, bool &isValid);
    bool computeValue(const Query &, ref<Expr> &result);
    bool computeInitialValues(const Query &,
                                const std::vector<const SymbolicExpr *> &objects,
                                std::vector<int32_t> &values);
    bool internalRunSolver(const Query &query, 
                            const std::vector<const SymbolicExpr *> *objects,
                            std::vector<int32_t> *values);
    void solveConstraint(const ref<Expr> &e, int32_t &res);
    SolverRunStatus getOperationStatusCode();
    int32_t generateRandomExcluding(const std::vector<int32_t>& cannot);
    std::pair<int32_t, int32_t> evaluate(ref<Expr> e);
};

TinySolverImpl::TinySolverImpl() {}

bool TinySolverImpl::computeValidity(const Query &query) {
    return internalRunSolver(query, NULL /* objects */, NULL /* values */);
}

bool TinySolverImpl::computeTruth(const Query &, bool &isValid) {
    std::cerr << "\033[31mDon't be tempted by GPT for code generation – our "
                << "friendly code is waiting for you!\033[0m" << std::endl;
    assert(false && "Assertion failed.");
}

bool TinySolverImpl::computeValue(const Query &, ref<Expr> &result) {
    std::cerr << "\033[31mDon't be tempted by GPT for code generation – our "
                << "friendly code is waiting for you!\033[0m" << std::endl;
    assert(false && "Assertion failed.");
}

bool TinySolverImpl::computeInitialValues(
    const Query &query, const std::vector<const SymbolicExpr *> &objects,
    std::vector<int32_t> &values) {
    assert(objects.size() <= 1 && "Currently support less two object");
    assert(values.empty() && "Currently support only one object");
    return internalRunSolver(query, &objects,  &values);
}

bool TinySolverImpl::internalRunSolver(
    const Query &query, const std::vector<const SymbolicExpr *> *objects,
    std::vector<int32_t> *values) {

    int32_t res;
    bool assigned = false; // Record whether the res is assigned
    std::vector<int32_t> cannot;

    // Deal with current branch condition
    auto cond = query.expr;
    if (cond->getKind() == Expr::Eq) {
        solveConstraint(query.expr, res);
        assigned = true;
    } else if (cond->getKind() == Expr::Not){
        int32_t cannotbe;
        solveConstraint(query.expr->getKid(0), cannotbe);
        cannot.push_back(cannotbe);
    } else if (cond->isTrue()){
        // Pass
    } else {
        assert(false && "This compare expression currently not support");
    }

    for (auto c : query.constraints) {
        if (c->getKind() == Expr::Eq) {
            int32_t prev;
            solveConstraint(c, prev);
            if (assigned && (res != prev)) return false;
            else {
                res = prev;
                assigned = true;
            }
        } else if (c->getKind() == Expr::Not) {
            int32_t cannotbe;
            solveConstraint(c->getKid(0), cannotbe);
            if (assigned && (res == cannotbe)) return false;
            cannot.push_back(cannotbe);
        } else assert(false && "This compare expression currently not support");
    }

    if (objects && values) {
        if (!assigned)
            res = generateRandomExcluding(cannot);
        values->push_back(res);

        llvm::errs() << "Assigning " << res << " " << "\n";
    }
    return true;
}

/// FIXME: This is a naive Implementation
void TinySolverImpl::solveConstraint(const ref<Expr> &e, int32_t &res) {
    assert(e->getKind() == Expr::Eq && "Constraint must be an equality");
    // Solving equaltation (X1 == X2)
    // where 
    //      X => X | X1 + X2 | X1 - X2
    //      Y => symbolic | constant 
    ref<EqExpr> eq = dyn_cast<EqExpr>(e.get());
    ref<Expr> left = e->getKid(0);
    ref<Expr> right = e->getKid(1);

    // Return pair: <Num of symblics, Value of constants>
    std::pair<int32_t, int32_t> p1 = evaluate(left);
    std::pair<int32_t, int32_t> p2 = evaluate(right);

    int valueConst = p2.first - p1.first;
    int numSym = p1.second - p2.second;

    if (numSym == 0)
        assert(valueConst == 0 && "Invalid expression");
    else {
        res = valueConst / numSym;
    }
}

std::pair<int32_t, int32_t> TinySolverImpl::evaluate(ref<Expr> e) {
    switch (e->getKind()) {
    case Expr::Constant: {
        auto *constant = dyn_cast<ConstantExpr>(e.get());
        return std::pair<int32_t, int32_t>(
                constant->getAPValue().getSExtValue(),
                0 /* Number of symbolic variables */);
        break;
    }
    case Expr::Symbolic: {
        return std::pair<int32_t, int32_t>(
                0 /* Value of constant */,
                1 /* Number of symbolic variables */);
        break;
    }
    case Expr::Add: {
        auto *add = dyn_cast<AddExpr>(e.get());
        auto p1 = evaluate(add->getKid(0));
        auto p2 = evaluate(add->getKid(1));
        return std::pair<int32_t, int32_t>(
                p1.first + p2.first,
                p1.second + p2.second);
        break;
    }
    case Expr::Sub: {
        auto *sub = dyn_cast<SubExpr>(e.get());
        auto p1 = evaluate(sub->getKid(0));
        auto p2 = evaluate(sub->getKid(1));
        return std::pair<int32_t, int32_t>(
                p1.first - p2.first,
                p1.second - p2.second);
        break;
    }
    default:
        assert(false && "unhandled expression kind");
    }
}

SolverImpl::SolverRunStatus TinySolverImpl::getOperationStatusCode() {
    return SOLVER_RUN_STATUS_FAILURE;
}

std::unique_ptr<Solver> createTinySolver() {
    return std::make_unique<Solver>(std::make_unique<TinySolverImpl>());
}

int32_t TinySolverImpl::generateRandomExcluding(const std::vector<int32_t>& cannot) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dist(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());

    int32_t v;
    do {
        v = dist(gen);
    } while (std::find(cannot.begin(), cannot.end(), v) != cannot.end());

    return v;
}

}
