#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"

#define INSTRUCTION(___opc, ___arg1, ___arg2) \
    OperationVector_append(ops, (Operation_init(___opc, ___arg1, ___arg2)))

#define ___INSTRUCTIONS(___ty, ___ast) \
    do { \
        OperationVector *___code = icodegen_##___ty(___ast, next_label); \
        OperationVector_append_all(ops, ___code); \
        OperationVector_free(___code); \
    } while(0)

#define FUNC_INSTRUCTIONS(___func) ___INSTRUCTIONS(Func, ___func)
#define COMM_INSTRUCTIONS(___comm) ___INSTRUCTIONS(Comm, ___comm)
#define EXPR_INSTRUCTIONS(___expr) ___INSTRUCTIONS(Expr, ___expr)

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
    OperationVector *ops = OperationVector_init();
    if(Comm_isWhile(comm)) {

        int guard_label = (*next_label)++;
        int end_label   = (*next_label)++;

        INSTRUCTION(LABEL   , guard_label   , 0             );
        EXPR_INSTRUCTIONS(While_guard(comm));
        INSTRUCTION(JUMPCOND, end_label     , ACCUMULATOR   );

        COMM_INSTRUCTIONS(While_body(comm));

        INSTRUCTION(JUMP    , guard_label   , 0             );
        INSTRUCTION(LABEL   , end_label     , 0             );
    }
    else if(Comm_isAssign(comm)) {
        EXPR_INSTRUCTIONS(Assign_expr(comm));
        INSTRUCTION(STORE, ACCUMULATOR, FRAME_POINTER + Assign_name(comm));
    }
    else if(Comm_isComp(comm)) {
        COMM_INSTRUCTIONS(Comp_fst(comm));
        COMM_INSTRUCTIONS(Comp_snd(comm));
    }
    else if(Comm_isReturn(comm)) {
    }
    else {
        ERROR("Comm type not recognised.");
    }
    return ops;
}

OperationVector *icodegen_Expr(Expr *expr, int *next_label) {
    OperationVector *ops = OperationVector_init();
    if(Expr_isInt(expr)) {
        INSTRUCTION(    LOADIMM , Int_value(expr)   , ACCUMULATOR   );
    }
    else if(Expr_isAdd(expr)) {

        // Evaluate the RHS
        EXPR_INSTRUCTIONS(Add_rhs(expr));

        // Push the RHS result on the stack
        INSTRUCTION(    STORE   , ACCUMULATOR       , STACK_POINTER );
        INSTRUCTION(    LOADIMM , 1                 , ACCUMULATOR   );
        INSTRUCTION(    ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Evaluate the LHS
        EXPR_INSTRUCTIONS(Add_lhs(expr));

        // Pop the RHS into temporary register
        INSTRUCTION(    LOADIMM , 1                 , TEMPORARY     );
        INSTRUCTION(    SUB     , STACK_POINTER     , TEMPORARY     );
        INSTRUCTION(    LOAD    , STACK_POINTER     , TEMPORARY     );

        // Do the addition
        INSTRUCTION(    ADD     , ACCUMULATOR       , TEMPORARY     );
    }
    else if(Expr_isSub(expr)) {

        // Evaluate the RHS
        EXPR_INSTRUCTIONS(Sub_rhs(expr));

        // Push the RHS result on the stack
        INSTRUCTION(    STORE   , ACCUMULATOR       , STACK_POINTER );
        INSTRUCTION(    LOADIMM , 1                 , ACCUMULATOR   );
        INSTRUCTION(    ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Evaluate the LHS
        EXPR_INSTRUCTIONS(Sub_lhs(expr));

        // Pop the RHS into temporary register
        INSTRUCTION(    LOADIMM , 1                 , TEMPORARY     );
        INSTRUCTION(    SUB     , STACK_POINTER     , TEMPORARY     );
        INSTRUCTION(    LOAD    , STACK_POINTER     , TEMPORARY     );

        // Do the subtraction
        INSTRUCTION(    SUB     , ACCUMULATOR       , TEMPORARY     );
    }
    else if(Expr_isCall(expr)) {
        //INSTRUCTION(  STORE   , , );

        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {

        }
    }
    else if(Expr_isVar(expr)) {
        INSTRUCTION(LOAD, FRAME_POINTER + Var_name(expr), ACCUMULATOR);
    }
    else {
        ERROR("Expr type not recognised.");
    }
    return ops;
}
