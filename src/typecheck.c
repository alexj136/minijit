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

Type *NoneType_init() {
    return Type_init(typeIDNone, NULL, NULL);
}

/*
 * Obtain a complete deep copy of a type
 */
Type *Type_copy(Type *type) {
    if(Type_isFuncType(type)) {
        TypeVector *argTypes = TypeVector_init();
        int idx;
        for(idx = 0; idx < TypeVector_size(FuncType_argTypes(type)); idx++) {
            TypeVector_append(argTypes,
                    Type_copy(TypeVector_get(FuncType_argTypes(type), idx)));
        }
        return Type_init(typeIDFunc, argTypes,
                Type_copy(FuncType_returnType(type)));
    }
    else { return Type_init(type->typeID, NULL, NULL); }
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

TypeError *TypeError_init(int name, Type *found, Type *expected,
        int src_line_no, int src_char_no) {

    TypeError *error = challoc(sizeof(TypeError));
    error->name = name;
    error->found = found;
    error->expected = expected;
    error->src_line_no = src_line_no;
    error->src_char_no = src_char_no;
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
 * Does not free the elements of the TypeErrorVector, but frees the vector
 * itself.
 */
void TypeCheckResult_free(TypeCheckResult *res) {
    Type_free(TypeCheckResult_type(res));
    TypeErrorVector_free(TypeCheckResult_errors(res));
    free(res);
    return;
}

/*
 * Frees as with TypeCheckResult_free(), but does not free the stored type.
 */
void TypeCheckResult_free_preserve_type(TypeCheckResult *res) {
    TypeErrorVector_free(TypeCheckResult_errors(res));
    free(res);
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
    NOT_IMPLEMENTED;
}

TypeCheckResult *check_Expr(Expr *expr, TypeVector *symbol_table) {

    if(Expr_isInt(expr)) {
        return TypeCheckResult_init(IntType_init(), TypeErrorVector_init());
    }
    else if(Expr_isAdd(expr) || Expr_isSub(expr)) {

        Type *type;

        // Obtain references to the left & right hand sides of the arithmetic
        // operation
        Expr *lhs;
        Expr *rhs;
        if(Expr_isAdd(expr)) {
            lhs = Add_lhs(expr);
            rhs = Add_rhs(expr);
        }
        else if(Expr_isSub(expr)) {
            lhs = Sub_lhs(expr);
            rhs = Sub_rhs(expr);
        }

        // Do a type check on each side
        TypeCheckResult *res_l = check_Expr(lhs, symbol_table);
        TypeCheckResult *res_r = check_Expr(rhs, symbol_table);

        TypeErrorVector *errors_l = TypeCheckResult_errors(res_l);
        TypeErrorVector *errors_r = TypeCheckResult_errors(res_r);

        Type *type_l = TypeCheckResult_type(res_l);
        Type *type_r = TypeCheckResult_type(res_r);

        // Concatenate the errors from each side
        TypeErrorVector *errors = TypeErrorVector_concat(errors_l, errors_r);

        // Add any new errors and set the correct type
        if(!Type_isIntType(type_l)) {
            TypeErrorVector_append(errors,
                    TypeError_init(-1, type_l, IntType_init(),
                    Expr_src_line_no(lhs), Expr_src_char_no(lhs)));
            type = NoneType_init();
        }
        if(!Type_isIntType(type_r)) {
            TypeErrorVector_append(errors,
                    TypeError_init(-1, type_r, IntType_init(),
                    Expr_src_line_no(rhs), Expr_src_char_no(rhs)));
            type = NoneType_init();
        }
        if(Type_isIntType(type_l) && Type_isIntType(type_r)) {
            type = IntType_init();
        }

        // Free the results from the lhs and rhs
        TypeCheckResult_free(res_l);
        TypeCheckResult_free(res_r);

        // Return the new results
        return TypeCheckResult_init(type, errors);
    }
    else if(Expr_isCall(expr)) {

        int name = Call_name(expr);
        Type *type_in_table = TypeVector_get(symbol_table, name);

        if(!Type_isFuncType(type_in_table)) {
            NOT_IMPLEMENTED;
            /*TypeErrorVector* errors = TypeErrorVector_init();
            TypeError *err = TypeError_init(-1, , FuncType_init(),
                    Expr_src_line_no(expr), Expr_src_char_no(expr));
            TypeErrorVector_append(errors, err);
            return TypeCheckResult_init(NoneType_init(), errors);*/
        }

        int declared_arity = FuncType_arity(type_in_table);
        int usage_arity = Call_num_args(expr);

        if(declared_arity != usage_arity) {
            return ({ NOT_IMPLEMENTED; NULL; });
        }

        TypeErrorVector *errors = TypeErrorVector_init();
        int idx;
        for(idx = 0; idx < usage_arity; idx++) {

            TypeCheckResult *arg_check_result =
                    check_Expr(Call_arg(expr, idx), symbol_table);
            Type *expected_arg_type = FuncType_argType(type_in_table, idx);
            Type *found_arg_type = TypeCheckResult_type(arg_check_result);

            TypeErrorVector_append_all(errors,
                    TypeCheckResult_errors(arg_check_result));

            if(Type_typeID(expected_arg_type) != Type_typeID(found_arg_type)) {
                TypeErrorVector_append(errors, TypeError_init(-1,
                        Type_copy(found_arg_type), Type_copy(expected_arg_type),
                        Expr_src_line_no(expr), Expr_src_char_no(expr)));
            }

            TypeCheckResult_free(arg_check_result);
        }

        return TypeCheckResult_init( ({ NOT_IMPLEMENTED; NULL; }) , errors);
    }
    else if(Expr_isVar(expr)) {
        // Return the type given by the symbol table, with an empty
        // TypeErrorVector
        return TypeCheckResult_init(
                Type_copy(TypeVector_get(symbol_table, Var_name(expr))),
                TypeErrorVector_init());
    }
    else {
        ERROR("Expr type not recognised");
    }
}
