#include "Domain.h"

//===----------------------------------------------------------------------===//
// Abstract Domain Implementation
//===----------------------------------------------------------------------===//

namespace dataflow {

Domain::Domain() { Value = Uninit; }
Domain::Domain(Element V) { Value = V; }

Domain *Domain::add(Domain *E1, Domain *E2) {
  if (E1->Value == Uninit || E2->Value == Uninit)
    return new Domain(Uninit);
  if (E1->Value == Zero && E2->Value == Zero)
    return new Domain(Zero);
  if (E1->Value == Zero && E2->Value == NonZero)
    return new Domain(NonZero);
  if (E1->Value == NonZero && E2->Value == Zero)
    return new Domain(NonZero);
  return new Domain(MaybeZero);
}

Domain *Domain::sub(Domain *E1, Domain *E2) {
  if (E1->Value == Uninit || E2->Value == Uninit)
    return new Domain(Uninit);
  if (E1->Value == Zero && E2->Value == Zero)
    return new Domain(Zero);
  if (E1->Value == Zero && E2->Value == NonZero)
    return new Domain(NonZero);
  if (E1->Value == NonZero && E2->Value == Zero)
    return new Domain(NonZero);
  return new Domain(MaybeZero);
}

Domain *Domain::mul(Domain *E1, Domain *E2) {
  if (E1->Value == Uninit || E2->Value == Uninit)
    return new Domain(Uninit);
  if (E1->Value == Zero || E2->Value == Zero)
    return new Domain(Zero);
  if (E1->Value == NonZero && E2->Value == NonZero)
    return new Domain(NonZero);
  return new Domain(MaybeZero);
}

Domain *Domain::div(Domain *E1, Domain *E2) {
  if (E1->Value == Uninit || E2->Value == Uninit)
    return new Domain(Uninit);
  if (E2->Value == Zero || E2->Value == MaybeZero)
    return new Domain(Uninit);
  if (E1->Value == NonZero)
    return new Domain(MaybeZero);
  if (E1->Value == Zero)
    return new Domain(Zero);
  return new Domain(MaybeZero);
}

Domain *Domain::join(Domain *E1, Domain *E2) {
  switch (E1->Value) {
  case Uninit:
    return new Domain(*E2);
  case NonZero:
    switch (E2->Value) {
    case Uninit:
    case NonZero:
      return new Domain(NonZero);
    case Zero:
    case MaybeZero:
      return new Domain(MaybeZero);
    }
  case Zero:
    switch (E2->Value) {
    case Uninit:
    case Zero:
      return new Domain(Zero);
    case NonZero:
    case MaybeZero:
      return new Domain(MaybeZero);
    }
  case MaybeZero:
    return new Domain(MaybeZero);
  }
}

bool Domain::equal(Domain E1, Domain E2) { return E1.Value == E2.Value; }

void Domain::print(raw_ostream &O) {
  switch (Value) {
  case Uninit:
    O << "Uninit   ";
    break;
  case NonZero:
    O << "NonZero  ";
    break;
  case Zero:
    O << "Zero     ";
    break;
  case MaybeZero:
    O << "MaybeZero";
    break;
  }
}

raw_ostream &operator<<(raw_ostream &O, Domain V) {
  V.print(O);
  return O;
}

}; // namespace dataflow
