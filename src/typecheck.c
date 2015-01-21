#include <stdio.h>
#include <stdlib.h>
#include "typecheck.h"

/*
 * Functions for types
 */

DEFINE_VECTORABLE(Type)

Type *Type_init(TypeID typeID, TypeVector *argTypes, Type *returnType) {
    Type *type = challoc(sizeof(type));
    type->typeID = typeID;
    type->argTypes = argTypes;
    type->returnType = returnType;
    return type;
}

Type *FuncType_init(TypeVector *argTypes, Type *returnType) {
    return Type_init(typeIDFunc, argTypes, returnType);
}

Type *IntType_init() {
    return Type_init(typeIDInt, NULL, NULL);
}

bool Type_eq(Type *t1, Type *t2) {
    if((!t1) || (!t2)) { return t1 == t2; } // null check
    else {
        return (Type_typeID(t1) == Type_typeID(t2))
                && Type_eq(FuncType_returnType(t1), FuncType_returnType(t2))
                && TypeVector_eq(FuncType_argTypes(t1), FuncType_argTypes(t2));
    }
}

void Type_free(Type *type) {
    if(!type) { return; }
    Type_free(FuncType_returnType(type));
    TypeVector_free(FuncType_argTypes(type));
    free(type);
    return;
}

/*
 * Functions for type errors
 */

TypeError *TypeError_init(int name, Type *found, Type *expected) {
    TypeError *error = challoc(sizeof(TypeError));
    error->name = name;
    error->found = found;
    error->expected = expected;
    return error;
}

bool TypeError_eq(TypeError *error1, TypeError *error2) {
    return error1 == error2;
}

void TypeError_free(TypeError *error) {
    Type_free(error->found);
    Type_free(error->expected);
    free(error);
    return;
}

/*
 * Functions for type check results
 */

DEFINE_VECTORABLE(TypeError)

TypeCheckResult *TypeCheckResult_init(Type *type, TypeErrorVector *errors) {
    TypeCheckResult *tcr = challoc(sizeof(TypeCheckResult));
    tcr->type = type;
    tcr->errors = errors;
    return tcr;
}

/*
 * Type checking functions for the 4 kinds of program structures
 */

TypeCheckResult *check_Prog(Prog *prog, TypeVector *symbol_table) {
    NOT_IMPLEMENTED;
}

TypeCheckResult *check_Func(Func *func, TypeVector *symbol_table) {
    NOT_IMPLEMENTED;
}

TypeCheckResult *check_Comm(Comm *comm, TypeVector *symbol_table) {
    NOT_IMPLEMENTED;
}

TypeCheckResult *check_Expr(Expr *expr, TypeVector *symbol_table) {

    TypeErrorVector *errors;
    Type *type;

    if(Expr_isInt(expr)) {
        errors = TypeErrorVector_init();
        type = IntType_init();
    }
    else if(Expr_isAdd(expr)) {
        NOT_IMPLEMENTED;
    }
    else if(Expr_isSub(expr)) {
        NOT_IMPLEMENTED;
    }
    else if(Expr_isCall(expr)) {
        NOT_IMPLEMENTED;
    }
    else if(Expr_isVar(expr)) {
        NOT_IMPLEMENTED;
    }
    else {
        ERROR("Expr type not recognised");
    }

    return TypeCheckResult_init(type, errors);
}
