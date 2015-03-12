#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"

OperationVector *icodegen_Prog(Prog *prog, int *next_label) {
    if(!prog) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Func(Func *func, int *next_label) {
    if(!func) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Comm(Comm *comm, int *next_label) {
    if(!comm) { NOT_IMPLEMENTED; }
    if(!next_label) { NOT_IMPLEMENTED; }
    NOT_IMPLEMENTED;
}

OperationVector *icodegen_Expr(Expr *expr, int *next_label) {
    OperationVector *ops = OperationVector_init();
    if(Expr_isInt(expr)) {
        OperationVector_append(ops,
                Operation_init(LOADIMM, Int_value(expr), ACCUMULATOR));
    }
    else if(Expr_isAdd(expr) || Expr_isSub(expr)) {
        OperationVector *lhs_code = icodegen_Expr(Add_lhs(expr), next_label);
        OperationVector *rhs_code = icodegen_Expr(Add_rhs(expr), next_label);

        OperationVector_append_all(ops, rhs_code);

        OperationVector_append(ops,
                Operation_init(STORE, ACCUMULATOR, STACK_POINTER));
        OperationVector_append(ops,
                Operation_init(LOADIMM, 1, ACCUMULATOR));
        OperationVector_append(ops,
                Operation_init(ADD, STACK_POINTER, ACCUMULATOR));

        OperationVector_append_all(ops, lhs_code);

        OperationVector_append(ops,
                Operation_init(LOADIMM, 1, TEMPORARY));
        OperationVector_append(ops,
                Operation_init(SUB, STACK_POINTER, TEMPORARY));
        OperationVector_append(ops,
                Operation_init(LOAD, STACK_POINTER, TEMPORARY));

        if(Expr_isAdd(expr)) {
            OperationVector_append(ops,
                    Operation_init(ADD, ACCUMULATOR, TEMPORARY));
        }
        else /* if(Expr_isSub(expr)) */ {
            OperationVector_append(ops,
                    Operation_init(SUB, ACCUMULATOR, TEMPORARY));
        }

        OperationVector_free(rhs_code);
        OperationVector_free(rhs_code);
    }
    else if(Expr_isCall(expr)) {
        //OperationVector_append(ops, Operation_init(STORE, , ));

        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {

        }
    }
    else if(Expr_isVar(expr)) {
        OperationVector_append(ops, Operation_init(
                LOAD, FRAME_POINTER + Var_name(expr), ACCUMULATOR));
    }
    else {
        ERROR("Expr type not recognised.");
    }
    return ops;
}
