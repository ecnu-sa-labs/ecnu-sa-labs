#ifdef ENABLE_Z3

#include "klee/Config/config.h"
#include "klee/Support/ErrorHandling.h"
#include "klee/Support/FileHandling.h"
#include "klee/Support/OptionCategories.h"

#include <csignal>


#include "Z3Solver.h"
#include "Z3Builder.h"

#include "Constraints.h"
// #include "klee/Expr/Assignment.h"
// #include "klee/Expr/ExprUtil.h"
#include "Solver.h"
#include "SolverImpl.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>

#include "llvm/Support/ErrorHandling.h"

namespace miniklee {

class Z3SolverImpl : public SolverImpl {
private:
    std::unique_ptr<Z3Builder> builder;
    time::Span timeout;
    SolverRunStatus runStatusCode;
    ::Z3_params solverParameters;
    // Parameter symbols
    ::Z3_symbol timeoutParamStrSymbol;

    bool internalRunSolver(const Query &,
                            const std::vector<const SymbolicExpr *> *objects,
                            std::vector<std::vector<unsigned char> > *values,
                            bool &hasSolution);
    bool validateZ3Model(::Z3_solver &theSolver, ::Z3_model &theModel);

public:
    Z3SolverImpl();
    ~Z3SolverImpl();

    std::string getConstraintLog(const Query &) override;
    void setCoreSolverTimeout(time::Span _timeout) override {
    timeout = _timeout;

    auto timeoutInMilliSeconds = static_cast<unsigned>((timeout.toMicroseconds() / 1000));
    if (!timeoutInMilliSeconds)
        timeoutInMilliSeconds = UINT_MAX;
    Z3_params_set_uint(builder->ctx, solverParameters, timeoutParamStrSymbol,
                        timeoutInMilliSeconds);
    }

    bool computeTruth(const Query &, bool &isValid) override;
    bool computeValue(const Query &, ref<Expr> &result) override;
    bool computeInitialValues(const Query &,
                            const std::vector<const SymbolicExpr *> &objects,
                            std::vector<std::vector<unsigned char>> &values,
                            bool &hasSolution) override;
    SolverRunStatus
    handleSolverResponse(::Z3_solver theSolver, ::Z3_lbool satisfiable,
                        const std::vector<const SymbolicExpr *> *objects,
                        std::vector<std::vector<unsigned char>> *values,
                        bool &hasSolution);
    SolverRunStatus getOperationStatusCode() override;
};

Z3SolverImpl::Z3SolverImpl() : builder(new Z3Builder()),
        runStatusCode(SOLVER_RUN_STATUS_FAILURE) {
    assert(builder && "unable to create Z3Builder");
    solverParameters = Z3_mk_params(builder->ctx);
    Z3_params_inc_ref(builder->ctx, solverParameters);
    timeoutParamStrSymbol = Z3_mk_string_symbol(builder->ctx, "timeout");
    setCoreSolverTimeout(timeout);

}

Z3SolverImpl::~Z3SolverImpl() {
    Z3_params_dec_ref(builder->ctx, solverParameters);
}

Z3Solver::Z3Solver() : Solver(std::make_unique<Z3SolverImpl>()) {}

std::string Z3Solver::getConstraintLog(const Query &query) {
    return impl->getConstraintLog(query);
}

void Z3Solver::setCoreSolverTimeout(time::Span timeout) {
    impl->setCoreSolverTimeout(timeout);
}

std::string Z3SolverImpl::getConstraintLog(const Query &query) {
    assert(false && "Not Implemented yet");
}

bool Z3SolverImpl::computeTruth(const Query &query, bool &isValid) {
    bool hasSolution = false; // to remove compiler warning
    bool status =
        internalRunSolver(query, /*objects=*/NULL, /*values=*/NULL, hasSolution);
    isValid = !hasSolution;
    return status;
}

bool Z3SolverImpl::computeValue(const Query &query, ref<Expr> &result) {
    assert(false && "Not Implemented yet");

    // REMOVEME
//   std::vector<const SymbolicExpr *> objects;
//     std::vector<std::vector<unsigned char> > values;
//     bool hasSolution;

//   // Find the object used in the expression, and compute an assignment
//   // for them.
//     findSymbolicObjects(query.expr, objects);
//     if (!computeInitialValues(query.withFalse(), objects, values, hasSolution))
//         return false;
//     assert(hasSolution && "state has invalid constraint set");

//   // Evaluate the expression with the computed assignment.
//     Assignment a(objects, values);
//     result = a.evaluate(query.expr);

//     return true;
}

bool Z3SolverImpl::computeInitialValues(
    const Query &query, const std::vector<const SymbolicExpr *> &objects,
    std::vector<std::vector<unsigned char> > &values, bool &hasSolution) {
    return internalRunSolver(query, &objects, &values, hasSolution);
}

bool Z3SolverImpl::internalRunSolver(
    const Query &query, const std::vector<const SymbolicExpr *> *objects,
    std::vector<std::vector<unsigned char> > *values, bool &hasSolution) {

    Z3_solver theSolver = Z3_mk_solver(builder->ctx);
    Z3_solver_inc_ref(builder->ctx, theSolver);
    Z3_solver_set_params(builder->ctx, theSolver, solverParameters);

    runStatusCode = SOLVER_RUN_STATUS_FAILURE;

    ConstantSymbolicExprFinder constant_arrays_in_query;
    for (auto const &constraint : query.constraints) {
        // REVIEW: Construct the builder
        Z3_solver_assert(builder->ctx, theSolver, builder->construct(constraint));
        constant_arrays_in_query.visit(constraint);
    }

    Z3ASTHandle z3QueryExpr =
        Z3ASTHandle(builder->construct(query.expr), builder->ctx);
    constant_arrays_in_query.visit(query.expr);

    for (auto const &constant_array : constant_arrays_in_query.results) {
        assert(builder->constant_array_assertions.count(constant_array) == 1 &&
            "Constant array found in query, but not handled by Z3Builder");
        for (auto const &arrayIndexValueExpr :
            builder->constant_array_assertions[constant_array]) {
        Z3_solver_assert(builder->ctx, theSolver, arrayIndexValueExpr);
        }
    }

    Z3_solver_assert(
        builder->ctx, theSolver,
        Z3ASTHandle(Z3_mk_not(builder->ctx, z3QueryExpr), builder->ctx));

    // REMOVEME
    // if (dumpedQueriesFile) {
    //     *dumpedQueriesFile << "; start Z3 query\n";
    //     *dumpedQueriesFile << Z3_solver_to_string(builder->ctx, theSolver);
    //     *dumpedQueriesFile << "(check-sat)\n";
    //     *dumpedQueriesFile << "(reset)\n";
    //     *dumpedQueriesFile << "; end Z3 query\n\n";
    //     dumpedQueriesFile->flush();
    // }

    ::Z3_lbool satisfiable = Z3_solver_check(builder->ctx, theSolver);
    runStatusCode = handleSolverResponse(theSolver, satisfiable, objects, values,
                                        hasSolution);

    Z3_solver_dec_ref(builder->ctx, theSolver);
    builder->clearConstructCache();

    if (runStatusCode == SolverImpl::SOLVER_RUN_STATUS_SUCCESS_SOLVABLE ||
        runStatusCode == SolverImpl::SOLVER_RUN_STATUS_SUCCESS_UNSOLVABLE) {
        return true; // success
    }
    if (runStatusCode == SolverImpl::SOLVER_RUN_STATUS_INTERRUPTED) {
        raise(SIGINT);
    }
    return false; // failed
}

SolverImpl::SolverRunStatus Z3SolverImpl::handleSolverResponse(
    ::Z3_solver theSolver, ::Z3_lbool satisfiable,
    const std::vector<const SymbolicExpr *> *objects,
    std::vector<std::vector<unsigned char> > *values, bool &hasSolution) {
    switch (satisfiable) {
    case Z3_L_TRUE: {
        hasSolution = true;
        if (!objects) {
        // No assignment is needed
        assert(values == NULL);
        return SolverImpl::SOLVER_RUN_STATUS_SUCCESS_SOLVABLE;
        }
        assert(values && "values cannot be nullptr");
        ::Z3_model theModel = Z3_solver_get_model(builder->ctx, theSolver);
        assert(theModel && "Failed to retrieve model");
        Z3_model_inc_ref(builder->ctx, theModel);
        values->reserve(objects->size());
        for (std::vector<const SymbolicExpr *>::const_iterator it = objects->begin(),
                                                        ie = objects->end();
            it != ie; ++it) {
        const SymbolicExpr *array = *it;
        std::vector<unsigned char> data;

        data.reserve(array->size);
        for (unsigned offset = 0; offset < array->size; offset++) {
            // We can't use Z3ASTHandle here so have to do ref counting manually
            ::Z3_ast arrayElementExpr;
            Z3ASTHandle initial_read = builder->getInitialRead(array, offset);

            __attribute__((unused))
            bool successfulEval =
                Z3_model_eval(builder->ctx, theModel, initial_read,
                            /*model_completion=*/true, &arrayElementExpr);
            assert(successfulEval && "Failed to evaluate model");
            Z3_inc_ref(builder->ctx, arrayElementExpr);
            assert(Z3_get_ast_kind(builder->ctx, arrayElementExpr) ==
                    Z3_NUMERAL_AST &&
                "Evaluated expression has wrong sort");

            int arrayElementValue = 0;
            __attribute__((unused))
            bool successGet = Z3_get_numeral_int(builder->ctx, arrayElementExpr,
                                                &arrayElementValue);
            assert(successGet && "failed to get value back");
            assert(arrayElementValue >= 0 && arrayElementValue <= 255 &&
                "Integer from model is out of range");
            data.push_back(arrayElementValue);
            Z3_dec_ref(builder->ctx, arrayElementExpr);
        }
        values->push_back(data);
        }

        // Validate the model if requested
        // if (Z3ValidateModels) {
        //   bool success = validateZ3Model(theSolver, theModel);
        //   if (!success)
        //     abort();
        // }

        Z3_model_dec_ref(builder->ctx, theModel);
        return SolverImpl::SOLVER_RUN_STATUS_SUCCESS_SOLVABLE;
    }
    case Z3_L_FALSE:
        hasSolution = false;
        return SolverImpl::SOLVER_RUN_STATUS_SUCCESS_UNSOLVABLE;
    case Z3_L_UNDEF: {
        ::Z3_string reason =
            ::Z3_solver_get_reason_unknown(builder->ctx, theSolver);
        if (strcmp(reason, "timeout") == 0 || strcmp(reason, "canceled") == 0 ||
            strcmp(reason, "(resource limits reached)") == 0) {
        return SolverImpl::SOLVER_RUN_STATUS_TIMEOUT;
        }
        if (strcmp(reason, "unknown") == 0) {
        return SolverImpl::SOLVER_RUN_STATUS_FAILURE;
        }
        if (strcmp(reason, "interrupted from keyboard") == 0) {
        return SolverImpl::SOLVER_RUN_STATUS_INTERRUPTED;
        }
        klee_warning("Unexpected solver failure. Reason is \"%s,\"\n", reason);
        abort();
    }
    default:
        llvm_unreachable("unhandled Z3 result");
    }
}

bool Z3SolverImpl::validateZ3Model(::Z3_solver &theSolver, ::Z3_model &theModel) {
    bool success = true;
    ::Z3_ast_vector constraints =
        Z3_solver_get_assertions(builder->ctx, theSolver);
    Z3_ast_vector_inc_ref(builder->ctx, constraints);

    unsigned size = Z3_ast_vector_size(builder->ctx, constraints);

    for (unsigned index = 0; index < size; ++index) {
    Z3ASTHandle constraint = Z3ASTHandle(
        Z3_ast_vector_get(builder->ctx, constraints, index), builder->ctx);

    ::Z3_ast rawEvaluatedExpr;
    __attribute__((unused))
    bool successfulEval =
        Z3_model_eval(builder->ctx, theModel, constraint,
                        /*model_completion=*/true, &rawEvaluatedExpr);
    assert(successfulEval && "Failed to evaluate model");

    // Use handle to do ref-counting.
    Z3ASTHandle evaluatedExpr(rawEvaluatedExpr, builder->ctx);

    Z3SortHandle sort =
        Z3SortHandle(Z3_get_sort(builder->ctx, evaluatedExpr), builder->ctx);
    assert(Z3_get_sort_kind(builder->ctx, sort) == Z3_BOOL_SORT &&
            "Evaluated expression has wrong sort");

    Z3_lbool evaluatedValue =
        Z3_get_bool_value(builder->ctx, evaluatedExpr);
    if (evaluatedValue != Z3_L_TRUE) {
        llvm::errs() << "Validating model failed:\n"
                    << "The expression:\n";
        constraint.dump();
        llvm::errs() << "evaluated to \n";
        evaluatedExpr.dump();
        llvm::errs() << "But should be true\n";
        success = false;
        }
    }

    if (!success) {
        llvm::errs() << "Solver state:\n" << Z3_solver_to_string(builder->ctx, theSolver) << "\n";
        llvm::errs() << "Model:\n" << Z3_model_to_string(builder->ctx, theModel) << "\n";
    }

    Z3_ast_vector_dec_ref(builder->ctx, constraints);
    return success;
}

SolverImpl::SolverRunStatus Z3SolverImpl::getOperationStatusCode() {
    return runStatusCode;
}
}
#endif // ENABLE_Z3
