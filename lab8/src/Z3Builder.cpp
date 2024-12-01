#ifdef ENABLE_Z3
#include "Z3Builder.h"

#include "Expr.h"
#include "Solver.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/CommandLine.h"

using namespace miniklee;

namespace miniklee {

Z3Builder::Z3Builder() : {
    Z3_config cfg = Z3_mk_config();
    // It is very important that we ask Z3 to let us manage memory so that
    // we are able to cache expressions and sorts.
    ctx = Z3_mk_context_rc(cfg);
    // Make sure we handle any errors reported by Z3.
    Z3_set_error_handler(ctx, custom_z3_error_handler);
    // When emitting Z3 expressions make them SMT-LIBv2 compliant
    Z3_set_ast_print_mode(ctx, Z3_PRINT_SMTLIB2_COMPLIANT);
    Z3_del_config(cfg);
}

void custom_z3_error_handler(Z3_context ctx, Z3_error_code ec) {
    ::Z3_string errorMsg = Z3_get_error_msg(ctx, ec); // Z3 > 4.4.1
    if (strcmp(errorMsg, "canceled") == 0) {
        return;
    }
    llvm::errs() << "Error: Incorrect use of Z3. [" << ec << "] " << errorMsg
                << "\n";
    Z3_close_log();
    // abort();
}

Z3Builder::~Z3Builder() {
    // Clear caches so exprs/sorts gets freed before the destroying context
    // they aren associated with.
    clearConstructCache();
    _arr_hash.clear();
    constant_array_assertions.clear();
    Z3_del_context(ctx);
}

Z3ASTHandle Z3Builder::construct(ref<Expr> e, int *width_out) {
    // REVIEW: We could potentially use Z3_simplify() here
    // to store simpler expressions.
    if (isa<ConstantExpr>(e)) {
        return constructActual(e, width_out);
    } else {
        int width;
        if (!width_out)
            width_out = &width;
        Z3ASTHandle res = constructActual(e, width_out);
        constructed.insert(std::make_pair(e, std::make_pair(res, *width_out)));
        return res;
    }
}

/** if *width_out!=1 then result is a bitvector,
    otherwise it is a bool */
Z3ASTHandle Z3Builder::constructActual(ref<Expr> e, int *width_out) {
    int width;
    if (!width_out)
        width_out = &width;

    switch (e->getKind()) {
    case Expr::Constant: {
        ConstantExpr *CE = cast<ConstantExpr>(e.get());
        *width_out = CE->getWidth();

        // Coerce to bool if necessary.
        if (*width_out == 1)
            return CE->isTrue() ? getTrue() : getFalse();

        // Fast path.
        if (*width_out <= 32)
            return bvConst32(*width_out, CE->getAPValue().getZExtValue(32));

        ref<ConstantExpr> Tmp = CE;
        Z3ASTHandle Res = bvConst64(64, Tmp->Extract(0, 64)->getZExtValue());
        while (Tmp->getWidth() > 64) {
        Tmp = Tmp->Extract(64, Tmp->getWidth() - 64);
        unsigned Width = std::min(64U, Tmp->getWidth());
        Res = Z3ASTHandle(
            Z3_mk_concat(ctx,
                        bvConst64(Width, Tmp->Extract(0, Width)->getZExtValue()),
                        Res),
            ctx);
        }
        return Res;
    }
    }

}

} // namespace miniklee

#endif // ENABLE_Z3