#ifndef util
#include "util.h"
#endif // util

#ifndef syntax
#include "syntax.h"
#endif // syntax

#ifndef typecheck
#define typecheck

/*
 * Types
 */

typedef enum { none, function, integer } TypeID;

typedef struct Type Type;
struct Type {
    TypeID tid;
    struct TypeVector *argTypes;
    struct Type *returnType;
};

FORWARD_DECLARE_VECTORABLE(Type)

/*
 * Type errors
 */

typedef struct TypeError TypeError;
struct TypeError {
    int name;
    struct Type *found;
    struct Type *expected;
};

FORWARD_DECLARE_VECTORABLE(TypeError)

/*
 * Type checking functions
 */

TypeErrorVector *check_Prog(Prog *prog, TypeVector *symbol_table);
TypeErrorVector *check_Func(Prog *prog, TypeVector *symbol_table);
TypeErrorVector *check_Comm(Prog *prog, TypeVector *symbol_table);
TypeErrorVector *check_Expr(Prog *prog, TypeVector *symbol_table);

#endif // typecheck
