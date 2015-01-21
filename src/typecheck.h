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

#define Type_isFuncType(type) type->typeID == typeIDFunc
#define Type_isIntType(type) type->typeID == typeIDInt

#define FuncType_arity(type) TypeVector_size(type->argTypes)
#define FuncType_argType(type, num) TypeVector_get(type->argTypes, num)
#define FuncType_returnType(func) type->returnType

typedef enum { typeIDFunc, typeIDInt } TypeID;

typedef struct Type Type;
struct Type {
    TypeID typeID;
    struct TypeVector *argTypes;
    struct Type *returnType;
};

FORWARD_DECLARE_VECTORABLE(Type)

Type *FuncType_init(TypeVector *argTypes, Type *returnType);
Type *IntType_init();

/*
 * Type errors
 */

#define TypeError_name(tyerr) tyerr->name
#define TypeError_found(tyerr) tyerr->found
#define TypeError_expected(tyerr) tyerr->expected

typedef struct TypeError TypeError;
struct TypeError {
    int name;
    struct Type *found;
    struct Type *expected;
};

FORWARD_DECLARE_VECTORABLE(TypeError)

TypeError *TypeError_init(int name, Type *found, Type *expected);

/*
 * Type checking results - a type and a vector of type errors
 */

#define TypeCheckResult_type(tcr) tcr->type
#define TypeCheckResult_errors(tcr) tcr->errors
#define TypeCheckResult_num_errors(tcr) TypeErrorVector_size(tcr->errors)
#define TypeCheckResult_error(tcr, num) TypeErrorVector_get(tcr->errors, num)

typedef struct TypeCheckResult TypeCheckResult;
struct TypeCheckResult {
    Type *type;
    TypeErrorVector *errors;
};

TypeCheckResult *TypeCheckResult_init(Type *type, TypeErrorVector *errors);

/*
 * Type checking functions
 */

TypeCheckResult *check_Prog(Prog *prog, TypeVector *symbol_table);
TypeCheckResult *check_Func(Func *func, TypeVector *symbol_table);
TypeCheckResult *check_Comm(Comm *comm, TypeVector *symbol_table);
TypeCheckResult *check_Expr(Expr *expr, TypeVector *symbol_table);

#endif // typecheck
