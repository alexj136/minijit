#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"

OperationVector *icodegen_Prog(Prog *prog, int *next_reg, int *next_label) {
    if(!prog) { NOT_IMPLEMENTED; }
    if(!next_reg) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Func(Func *func, int *next_reg, int *next_label) {
    if(!func) { NOT_IMPLEMENTED; }
    if(!next_reg) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Comm(Comm *comm, int *next_reg, int *next_label) {
    if(!comm) { NOT_IMPLEMENTED; }
    if(!next_reg) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Expr(Expr *expr, int *next_reg, int *next_label) {
    OperationVector *ops = OperationVector_init();
    if(Expr_isInt(expr)) {
        OperationVector_append(ops,
                Operation_init(LOADIMM, Int_value(expr), *next_reg));
    }
    else if(Expr_isAdd(expr) || Expr_isSub(expr)) {
        OperationVector *lhs_code =
                icodegen_Expr(Add_lhs(expr), next_reg, next_label);
        (*next_reg)++;
        OperationVector *rhs_code =
                icodegen_Expr(Add_rhs(expr), next_reg, next_label);

        OperationVector_append_all(ops, lhs_code);
        OperationVector_append_all(ops, rhs_code);
        OperationVector_append(ops, Operation_init(Expr_isAdd(expr) ? ADD : SUB,
                (*next_reg)-1, *next_reg));

        (*next_reg)--;
        OperationVector_free(rhs_code);
        OperationVector_free(rhs_code);
    }
    else if(Expr_isCall(expr)) {
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {
        }
    }
    else if(Expr_isVar(expr)) {
    }
    else {
        ERROR("Expr type not recognised.");
    }
    return ops;
}
