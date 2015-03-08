#include <stdio.h>
#include <stdlib.h>
#include "typecheck.h"

/*
 * Functions for types
 */

DEFINE_VECTORABLE(Type)

Type *Type_init(TypeForm form, Type *innerType, TypeVector *innerTypes) {
    Type *type = challoc(sizeof(type));
    type->form = form;
    type->innerType = innerType;
    type->innerTypes = innerTypes;
    return type;
}

Type *FuncType_init(TypeVector *argTypes, Type *returnType) {
    return Type_init(typeFunc, returnType, argTypes);
}

Type *IntType_init() {
    return Type_init(typeInt, NULL, NULL);
}

Type *VoidType_init() {
    return Type_init(typeVoid, NULL, NULL);
}

/*
 * Obtain a complete deep copy of a type
 */
Type *Type_copy(Type *old) {
    Type *new       = challoc(sizeof(Type));
    new->form       = old->form;
    new->innerType  = (old->innerType)  ? Type_copy(old->innerType)  : NULL;
    if(old->innerTypes) {
        new->innerTypes = TypeVector_init();
        int idx;
        for(idx = 0; idx < TypeVector_size(old->innerTypes); idx++) {
            TypeVector_append(new->innerTypes,
                    Type_copy(TypeVector_get(old->innerTypes, idx)));
        }
    }
    else {
        new->innerTypes = NULL;
    }
    return new;
}

bool Type_eq(Type *t1, Type *t2) {
    return ( // Do the forms match?
        t1->form == t2->form
    )
    && ( // Does the inner type match, checking for null?
        ((t1->innerType == NULL) || (t2->innerType == NULL))
                ? t1 == t2
                : Type_eq(t1->innerType, t2->innerType)
    )
    && ( // Do the inner types all match in type and sequence?
        ((t1->innerType == NULL) || (t2->innerType == NULL))
                ? t1 == t2
                : TypeVector_eq(t1->innerTypes, t2->innerTypes)
    );
}

void Type_free(Type *type) {
    if(type->innerType) { Type_free(type->innerType); }
    if(type->innerTypes) { TypeVector_free(type->innerTypes); }
    free(type);
    return;
}

/*
 * Functions for type errors
 */

DEFINE_VECTORABLE(TypeError)

TypeError *TypeError_init(TypeErrorInfo info, int name, Type *found,
        Type *expected, int line_no, int char_no) {

    TypeError *error = challoc(sizeof(TypeError));
    error->info      = info;
    error->name      = name;
    error->found     = found;
    error->expected  = expected;
    error->line_no   = line_no;
    error->char_no   = char_no;
    return error;
}

bool TypeError_eq(TypeError *error1, TypeError *error2) {
    // Null checks are essentially to supress the unused parameter warning
    if(!error1) { ERROR("Null TypeError"); }
    if(!error2) { ERROR("Null TypeError"); }
    ERROR("Equality not defined for TypeErrors");
}

void TypeError_free(TypeError *error) {
    Type_free(error->found);
    Type_free(error->expected);
    free(error);
    return;
}

/*
 * Functions for TypeCheckResults
 */

TypeCheckResult *TypeCheckSuccess_init(Type *type) {
    TypeCheckResult *res = challoc(sizeof(TypeCheckResult));
    res->success = true;
    res->type    = type;
    res->errors  = NULL;
    return res;
}

TypeCheckResult *TypeCheckFail_init(TypeErrorVector *errors) {
    TypeCheckResult *res = challoc(sizeof(TypeCheckResult));
    res->success = false;
    res->type    = NULL;
    res->errors  = errors;
    return res;
}

bool TypeCheckResult_isTypeForm(TypeCheckResult *result, TypeForm form) {
    return (result->success) && ((result->type->form) == form);
}

/*
 * Does not free the elements of the TypeErrorVector as they will be passed up
 * to the first check call
 */
void TypeCheckResult_free(TypeCheckResult *result) {
    if(result->success) { Type_free(result->type); }
    else { TypeErrorVector_free(result->errors); }
    free(result);
    return;
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
    if(Comm_isWhile(comm)) {}
    else if(Comm_isAssign(comm)) {}
    else if(Comm_isReturn(comm)) {

    }
    else if(Comm_isComp(comm)) {
        TypeCheckResult *fst_res = check_Comm(Comp_fst(comm), symbol_table);
    }
    else { ERROR("Unrecognised Comm type"); }
    NOT_IMPLEMENTED;
}

TypeCheckResult *check_Expr(Expr *expr, TypeVector *symbol_table) {
    if(Expr_isInt(expr)) {
        return TypeCheckSuccess_init(IntType_init());
    }
    else if(Expr_isAdd(expr) || Expr_isSub(expr)) {
        TypeCheckResult *lhs_res = check_Expr(Add_lhs(expr), symbol_table);
        TypeCheckResult *rhs_res = check_Expr(Add_rhs(expr), symbol_table);

        // Success case
        if(TypeCheckResult_isTypeForm(lhs_res, typeInt) &&
                TypeCheckResult_isTypeForm(rhs_res, typeInt)) {

            TypeCheckResult_free(lhs_res);
            return rhs_res;
        }

        // Fail case
        TypeErrorVector *errors = TypeErrorVector_init();
        if(!(lhs_res->success)) {
            TypeErrorVector_append_all(errors, lhs_res->errors);
        }
        if(!(rhs_res->success)) {
            TypeErrorVector_append_all(errors, rhs_res->errors);
        }

        TypeCheckResult_free(lhs_res);
        TypeCheckResult_free(rhs_res);
        return TypeCheckFail_init(errors);
    }
    else if(Expr_isCall(expr)) {}
    else if(Expr_isVar(expr)) {
        return TypeCheckSuccess_init(
                Type_copy(TypeVector_get(symbol_table, Var_name(expr))));
    }
    else { ERROR("Unrecognised Expr type"); }
    NOT_IMPLEMENTED;
}
