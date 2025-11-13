#ifdef ENABLE_Z3

#ifndef Z3SOLVER_H
#define Z3SOLVER_H

#include "Solver.h"

namespace miniklee {
/// Z3Solver - A complete solver based on Z3
class Z3Solver : public Solver {
public:
    /// Z3Solver - Construct a new Z3Solver.
    Z3Solver();

    /// Get the query in SMT-LIBv2 format.
    /// \return A C-style string. The caller is responsible for freeing this.
    std::string getConstraintLog(const Query &) override;

    /// setCoreSolverTimeout - Set constraint solver timeout delay to the given
    /// value; 0 is off.
    void setCoreSolverTimeout(time::Span timeout) override;
};
}

#endif /* Z3SOLVER_H */

#endif // ENABLE_Z3