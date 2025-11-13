#ifndef EXPR_H
#define EXPR_H

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include "Ref.h"

namespace miniklee {
class Expr {
public:
    static unsigned count;
    static const unsigned MAGIC_HASH_CONSTANT = 39;

    // The type of an expression is simply its width, in bits. 
    typedef unsigned Width;

    // Currently only 8-bit integers are supported.
    static const Width Bool = 1;
    static const Width Int32 = 32;

    enum Kind {
        InvalidKind = -2,

        // Primitive
        
        Symbolic = -1,
        Constant = 0,

        // Bit
        Not,

        // Arithmetic
        Add,
        Sub,
        Mul,
        UDiv,
        SDiv,

        // Compare
        Eq,
        Ne,  ///< Not used in canonical form
        Ult,
        Ule,
        Ugt, ///< Not used in canonical form
        Uge, ///< Not used in canonical form
        Slt,
        Sle,
        Sgt, ///< Not used in canonical form
        Sge, ///< Not used in canonical form

        LastKind = Sge,

        BinaryKindFirst = Add,
        BinaryKindLast = SDiv,
        CmpKindFirst = Eq,
        CmpKindLast = Sge
    };

    /// @brief Required by klee::ref-managed objects
    class ReferenceCounter _refCount;

public:
    unsigned hashValue;
    Expr() { Expr::count++; }

    virtual ~Expr() { Expr::count--; }

    virtual Kind getKind() const = 0;
    virtual Width getWidth() const = 0;

    virtual unsigned getNumKids() const = 0;
    virtual ref<Expr> getKid(unsigned i) const = 0;

    /// dump - Print the expression to stderr.
    void dump() const;

    // REVIEW: Construct debug components such as print and dump
    //      to make develop life easier ?
    // virtual void print(llvm::raw_ostream& os) const;

    /// dump - Print the expression to stderr.
    // void dump() const;
      /// Returns the pre-computed hash of the current expression
    virtual unsigned hash() const { return hashValue; }

    /// (Re)computes the hash of the current expression.
    /// Returns the hash value. 
    virtual unsigned computeHash();

    /// isZero - Is this a constant zero.
    bool isZero() const;
    
    /// isTrue - Is this the true expression.
    bool isTrue() const;

    /// isFalse - Is this the false expression.
    bool isFalse() const;

    static ref<Expr> createIsZero(ref<Expr> e);

    static void printKind(llvm::raw_ostream& os, Kind k);
    static void printWidth(llvm::raw_ostream& os, Width w);

};

class NonConstantExpr : public Expr {
public:
    static bool classof(const Expr* e) {
        return e->getKind() != Expr::Constant;
    }
    static bool classof(const NonConstantExpr *) { return true; }
};

class BinaryExpr : public NonConstantExpr {
public:
    ref<Expr> left, right;

public:
    unsigned getNumKids() const { return 2; }

    ref<Expr> getKid(unsigned i) const {
        switch (i) {
        case 0: return left;
        case 1: return right;
        default: return 0;
        }
    }
    protected:
    BinaryExpr(const ref<Expr> &l, const ref<Expr> &r) : left(l), right(r) {}

public:
    static bool classof(const Expr *E) {
        Kind k = E->getKind();
        return Expr::BinaryKindFirst <= k && k <= Expr::BinaryKindLast;
    }
    static bool classof(const BinaryExpr *) { return true; }
};

class CmpExpr : public BinaryExpr {

protected:
    CmpExpr(ref<Expr> l, ref<Expr> r) : BinaryExpr(l,r) {}
    
    public:                                                       
    Width getWidth() const { return Bool; }

    static bool classof(const Expr *E) {
        Kind k = E->getKind();
        return Expr::CmpKindFirst <= k && k <= Expr::CmpKindLast;
    }
    static bool classof(const CmpExpr *) { return true; }
};


class NotExpr : public NonConstantExpr { 
public:
    static const Kind kind = Not;
    static const unsigned numKids = 1;
    
    ref<Expr> expr;

    public:  
    static ref<Expr> alloc(const ref<Expr> &e) {
        ref<Expr> r(new NotExpr(e));
        r->computeHash();
        return r;
    }
    
    static ref<Expr> create(const ref<Expr> e);

    Width getWidth() const { return expr->getWidth(); }
    Kind getKind() const { return Not; }

    unsigned getNumKids() const { return numKids; }
    ref<Expr> getKid(unsigned i) const { return expr; }

    virtual ref<Expr> rebuild(ref<Expr> kids[]) const { 
        return create(kids[0]);
    }

    virtual unsigned computeHash();

    public:
    static bool classof(const Expr *E) {
        return E->getKind() == Expr::Not;
    }
  static bool classof(const NotExpr *) { return true; }

private:
    NotExpr(const ref<Expr> &e) : expr(e) {}

protected:
    virtual int compareContents(const Expr &b) const {
        // No attributes to compare.
        return 0;
    }
};

// Arithmetic/Bit Exprs

#define ARITHMETIC_EXPR_CLASS(_class_kind)                                     \
class _class_kind##Expr : public BinaryExpr {                                \
public:                                                                      \
    static const Kind kind = _class_kind;                                      \
    static const unsigned numKids = 2;                                         \
public:                                                                      \
    _class_kind##Expr(const ref<Expr> &l, const ref<Expr> &r)                  \
        : BinaryExpr(l, r) {}                                                  \
    static ref<Expr> alloc(const ref<Expr> &l, const ref<Expr> &r) {           \
    ref<Expr> res(new _class_kind##Expr(l, r));                              \
    res->computeHash();                                                      \
    return res;                                                              \
    }                                                                          \
    static ref<Expr> create(const ref<Expr> l, const ref<Expr> r);           \
    Width getWidth() const { return left->getWidth(); }                        \
    Kind getKind() const { return _class_kind; }                               \
    virtual ref<Expr> rebuild(ref<Expr> kids[]) const {                        \
    return create(kids[0], kids[1]);                                         \
    }                                                                          \
    static bool classof(const Expr *E) {                                       \
    return E->getKind() == Expr::_class_kind;                                \
    }                                                                          \
    static bool classof(const _class_kind##Expr *) { return true; }            \
    protected:                                                                   \
    virtual int compareContents(const Expr &b) const {                         \
    /* No attributes to compare.*/                                           \
    return 0;                                                                \
    }                                                                          \
};

ARITHMETIC_EXPR_CLASS(Add)
ARITHMETIC_EXPR_CLASS(Sub)
ARITHMETIC_EXPR_CLASS(Mul)
ARITHMETIC_EXPR_CLASS(UDiv)
ARITHMETIC_EXPR_CLASS(SDiv)

// Comparison Exprs

#define COMPARISON_EXPR_CLASS(_class_kind)                                     \
class _class_kind##Expr : public CmpExpr {                                   \
public:                                                                      \
    static const Kind kind = _class_kind;                                      \
    static const unsigned numKids = 2;                                         \
public:                                                                      \
    _class_kind##Expr(const ref<Expr> &l, const ref<Expr> &r)                  \
        : CmpExpr(l, r) {}                                                     \
    static ref<Expr> alloc(const ref<Expr> &l, const ref<Expr> &r) {           \
    ref<Expr> res(new _class_kind##Expr(l, r));                              \
    res->computeHash();                                                      \
    return res;                                                              \
    }                                                                          \
    static ref<Expr> create(const ref<Expr> l, const ref<Expr> r);           \
    Kind getKind() const { return _class_kind; }                               \
    virtual ref<Expr> rebuild(ref<Expr> kids[]) const {                        \
    return create(kids[0], kids[1]);                                         \
    }                                                                          \
    static bool classof(const Expr *E) {                                       \
    return E->getKind() == Expr::_class_kind;                                \
    }                                                                          \
    static bool classof(const _class_kind##Expr *) { return true; }            \
protected:                                                                   \
    virtual int compareContents(const Expr &b) const {                         \
      /* No attributes to compare. */                                          \
    return 0;                                                                \
    }                                                                          \
};

COMPARISON_EXPR_CLASS(Eq)
COMPARISON_EXPR_CLASS(Ne)
COMPARISON_EXPR_CLASS(Ult)
COMPARISON_EXPR_CLASS(Ule)
COMPARISON_EXPR_CLASS(Ugt)
COMPARISON_EXPR_CLASS(Uge)
COMPARISON_EXPR_CLASS(Slt)
COMPARISON_EXPR_CLASS(Sle)
COMPARISON_EXPR_CLASS(Sgt)
COMPARISON_EXPR_CLASS(Sge)


// Terminal Exprs

#define TERMINAL_EXPR_CLASS(_class_kind)                                     \
class _class_kind##Expr : public Expr {\
public:\
    static const Kind kind = _class_kind;\
    static const unsigned numKids = 0;\
\
private:\
    llvm::APInt value;\
    _class_kind##Expr(const llvm::APInt &v) : value(v) {}\
\
public:\
    ~_class_kind##Expr() {}\
\
    Width getWidth() const { return value.getBitWidth(); }\
    Kind getKind() const { return _class_kind; }\
\
    unsigned getNumKids() const { return 0; }\
    ref<Expr> getKid(unsigned i) const { return 0; }\
\
    const llvm::APInt &getAPValue() const { return value; }\
\
    virtual unsigned computeHash();\
\
    static ref<_class_kind##Expr> alloc(const llvm::APInt &v) {\
        ref<_class_kind##Expr> r(new _class_kind##Expr(v));\
        r->computeHash();\
        return r;\
    }\
\
    static ref<_class_kind##Expr> alloc(const llvm::APFloat &f) {\
        return alloc(f.bitcastToAPInt());\
    }\
\
    static ref<_class_kind##Expr> alloc(uint64_t v, Width w) {\
        return alloc(llvm::APInt(w, v));\
    }\
\
    static ref<_class_kind##Expr> create(uint64_t v, Width w) {\
        return alloc(v, w);\
    }\
\
    static bool classof(const Expr *E) { return E->getKind() == Expr::_class_kind; }\
    static bool classof(const _class_kind##Expr *) { return true; }\
\
    bool isZero() const { return getAPValue().isMinValue(); }\
\
    bool isTrue() const {\
        return (getWidth() == Expr::Int32 && value.getSExtValue() != 0);\
    }\
\
    bool isFalse() const {\
        return (getWidth() == Expr::Int32 && value.getSExtValue() == 0);\
    }\
\
    bool isAllOnes() const {\
        return getAPValue().isAllOnes();\
    }\
\
    ref<_class_kind##Expr> Not();\
};\

TERMINAL_EXPR_CLASS(Constant)
TERMINAL_EXPR_CLASS(InvalidKind)

class SymbolicExpr: public Expr {
public: static
    const Kind kind = Symbolic;
    static const unsigned numKids = 0;

private:
    const std::string name;

    SymbolicExpr(std::string n): name(n) {}

    public: ~SymbolicExpr() {}

    Kind getKind() const { return Symbolic; }
    
    unsigned getNumKids() const { return 0; }

    Width getWidth() const { return Int32; }

    ref <Expr> getKid(unsigned i) const {
        return 0;
    }

    const std::string getName() const { return name; }

    virtual unsigned computeHash();
    
    static ref<SymbolicExpr> alloc(std::string n) {
        ref <SymbolicExpr> r(new SymbolicExpr(n));
        r -> computeHash();
        return r;
    }

    static ref< SymbolicExpr > create(std::string n) {
        return alloc(n);
    }

    static bool classof(const Expr * E) {
        return E -> getKind() == Expr::Symbolic;
    }

    static bool classof(const SymbolicExpr * ) { return true; }

    bool isZero() const { return false; }
    bool isTrue() const { return true; }
    bool isFalse() const { return false; }
};

// Implementations
inline bool Expr::isZero() const {
if (const ConstantExpr *CE = llvm::dyn_cast<ConstantExpr>(this))
    return CE->isZero();
return false;
}

inline bool Expr::isTrue() const {
assert(getWidth() == Expr::Int32 && "Invalid isTrue() call!");
if (const ConstantExpr *CE = llvm::dyn_cast<ConstantExpr>(this))
    return CE->isTrue();
return false;
}

inline bool Expr::isFalse() const {
assert(getWidth() == Expr::Int32 && "Invalid isFalse() call!");
if (const ConstantExpr *CE = llvm::dyn_cast<ConstantExpr>(this))
    return CE->isFalse();
return false;
}

}

#endif // EXPR_H