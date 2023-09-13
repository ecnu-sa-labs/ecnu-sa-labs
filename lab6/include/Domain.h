#ifndef DOMAIN_H
#define DOMAIN_H

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace dataflow {

//===----------------------------------------------------------------------===//
// Abstract Domain Implementation
//===----------------------------------------------------------------------===//

/*
 * Implement your abstract domain.
 */
class Domain {
public:
  enum Element { Uninit, NonZero, Zero, MaybeZero };
  Domain();
  Domain(Element V);
  Element Value;

  static Domain *add(Domain *E1, Domain *E2);
  static Domain *sub(Domain *E1, Domain *E2);
  static Domain *mul(Domain *E1, Domain *E2);
  static Domain *div(Domain *E1, Domain *E2);
  static Domain *join(Domain *E1, Domain *E2);
  static bool equal(Domain E1, Domain E2);
  void print(raw_ostream &O);
};

raw_ostream &operator<<(raw_ostream &O, Domain V);

} // namespace dataflow

#endif // DOMAIN_H
