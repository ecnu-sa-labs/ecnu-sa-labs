/**
 * @file Domain.cpp
 * @brief Abstract Domain.
 */

#include "Domain.h"

namespace dataflow {

/**
 * @brief Default constructor for Domain class.
 */
Domain::Domain() { Value = Uninit; }

/**
 * @brief Parameterized constructor for Domain class.
 *
 * @param V Initial value for domain element.
 */
Domain::Domain(Element V) { Value = V; }

/**
 * @brief Add two domain elements.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return New domain element representing addition result.
 */
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

/**
 * @brief Subtract two domain elements.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return New domain element representing subtraction result.
 */
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

/**
 * @brief Multiply two domain elements.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return New domain element representing multiplication result.
 */
Domain *Domain::mul(Domain *E1, Domain *E2) {
  if (E1->Value == Uninit || E2->Value == Uninit)
    return new Domain(Uninit);

  if (E1->Value == Zero || E2->Value == Zero)
    return new Domain(Zero);

  if (E1->Value == NonZero && E2->Value == NonZero)
    return new Domain(NonZero);

  return new Domain(MaybeZero);
}

/**
 * @brief Divide two domain elements.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return New domain element representing division result.
 */
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

/**
 * @brief Join two domain elements.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return New domain element representing join result.
 */
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

/**
 * @brief Check if two domain elements are equal.
 *
 * @param E1 1st domain element.
 * @param E2 2nd domain element.
 * @return true if elements are equal, false otherwise.
 */
bool Domain::equal(Domain E1, Domain E2) { return E1.Value == E2.Value; }

/**
 * @brief Print domain element value.
 *
 * @param O Output stream to print to.
 */
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

/**
 * @brief Overloaded stream insertion operator for Domain class.
 *
 * @param O Output stream.
 * @param V Domain element to print.
 * @return Output stream.
 */
raw_ostream &operator<<(raw_ostream &O, Domain V) {
  V.print(O);
  return O;
}

}; // namespace dataflow
