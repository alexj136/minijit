#include <stdio.h>
#include <stdlib.h>
#include "typecheck.h"

/*
 * Functions for types
 */

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

/*
 * Functions for type errors
 */

TypeError *TypeError_init(int name, Type *found, Type *expected) {
    TypeError *tyerr = challoc(sizeof(TypeError));
    tyerr->name = name;
    tyerr->found = found;
    tyerr->expected = expected;
    return tyerr;
}

/*
 * Functions for type check results
 */

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
