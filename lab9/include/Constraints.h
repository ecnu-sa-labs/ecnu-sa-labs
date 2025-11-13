#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "Expr.h"

namespace miniklee {

/// Resembles a set of constraints that can be passed around
///
class ConstraintSet {
public:
    using constraints_ty = std::vector<ref<Expr>>;
    using iterator = constraints_ty::iterator;
    using const_iterator = constraints_ty::const_iterator;

    using constraint_iterator = const_iterator;

    bool empty() const { return constraints.empty(); }
    constraint_iterator begin() const { return constraints.begin(); }
    constraint_iterator end() const { return constraints.end(); }
    size_t size() const noexcept { return constraints.size(); }

    void push_back(const ref<Expr> &e) { constraints.push_back(e); }

    explicit ConstraintSet(constraints_ty cs) : constraints(std::move(cs)) {}
    ConstraintSet() = default;

    private:
        constraints_ty constraints;
    };
} // miniklee

#endif