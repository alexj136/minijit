#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"

/*
 * Macros to make code generation functions more readable
 */

#define BEGIN_CODE_GENERATION \
    OperationVector *ops = OperationVector_init()

#define INSERT_OPERATION(___opc, ___arg1, ___arg2) \
    OperationVector_append(ops, (Operation_init(___opc, ___arg1, ___arg2)))

#define INSERT_FUNC_CODE(___func) ___INSERT_CODE(Func, ___func)
#define INSERT_COMM_CODE(___comm) ___INSERT_CODE(Comm, ___comm)
#define INSERT_EXPR_CODE(___expr) ___INSERT_CODE(Expr, ___expr)
#define ___INSERT_CODE(___ty, ___ast) \
    do { \
        OperationVector *___code = icodegen_##___ty(___ast, next_label); \
        OperationVector_append_all(ops, ___code); \
        OperationVector_free(___code); \
    } while(0)

#define RETURN_GENERATED_CODE \
    return ops

/*
 * Code generation functions - (accumulator strategy)
 */

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

    BEGIN_CODE_GENERATION;

    if(Comm_isWhile(comm)) {

        // Label names
        int guard_label = (*next_label)++;
        int end_label   = (*next_label)++;

        // Guard label & guard evaluation
        INSERT_OPERATION(   LABEL   , guard_label       , 0             );
        INSERT_EXPR_CODE(   While_guard(comm));

        // Jump straight to the end if the guard was less than 1
        INSERT_OPERATION(   JUMPCOND, end_label         , ACCUMULATOR   );

        // If we didn't jump to the end, run the code for the body and jump back
        // to the guard evaluation
        INSERT_COMM_CODE(   While_body(comm)                            );
        INSERT_OPERATION(   JUMP    , guard_label       , 0             );

        // End label
        INSERT_OPERATION(   LABEL   , end_label         , 0             );
    }
    else if(Comm_isAssign(comm)) {

        // Evaluate the expression
        INSERT_EXPR_CODE(   Assign_expr(comm)                           );

        // Calculate the address to store the value
        INSERT_OPERATION(   LOADIMM , Assign_name(comm) , TEMPORARY     );
        INSERT_OPERATION(   ADD     , TEMPORARY         , FRAME_POINTER );

        // Store the value at the calculated address
        INSERT_OPERATION(   STORE   , ACCUMULATOR       , TEMPORARY     );
    }
    else if(Comm_isComp(comm)) {

        // Just insert code for the first and second command
        INSERT_COMM_CODE(   Comp_fst(comm)                              );
        INSERT_COMM_CODE(   Comp_snd(comm)                              );
    }
    else if(Comm_isReturn(comm)) {

        // Evaluate the expression
        INSERT_EXPR_CODE(   Assign_expr(comm)                           );

        // Return to caller
        INSERT_OPERATION(   JUMP    , RETURN_ADDRESS    , 0             );
    }
    else {
        ERROR("Comm type not recognised.");
    }

    RETURN_GENERATED_CODE;
}

OperationVector *icodegen_Expr(Expr *expr, int *next_label) {

    BEGIN_CODE_GENERATION;

    if(Expr_isInt(expr)) {

        // Just load the literal value into the accumulator
        INSERT_OPERATION(   LOADIMM , Int_value(expr)   , ACCUMULATOR   );
    }
    else if(Expr_isAdd(expr)) {

        // Evaluate the RHS
        INSERT_EXPR_CODE(Add_rhs(expr));

        // Push the RHS result on the stack
        INSERT_OPERATION(   STORE   , ACCUMULATOR       , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Evaluate the LHS
        INSERT_EXPR_CODE(Add_lhs(expr));

        // Pop the RHS into temporary register
        INSERT_OPERATION(   LOADIMM , 1                 , TEMPORARY     );
        INSERT_OPERATION(   SUB     , STACK_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   LOAD    , STACK_POINTER     , TEMPORARY     );

        // Do the addition
        INSERT_OPERATION(   ADD     , ACCUMULATOR       , TEMPORARY     );
    }
    else if(Expr_isSub(expr)) {

        // Evaluate the RHS
        INSERT_EXPR_CODE(Sub_rhs(expr));

        // Push the RHS result on the stack
        INSERT_OPERATION(   STORE   , ACCUMULATOR       , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Evaluate the LHS
        INSERT_EXPR_CODE(Sub_lhs(expr));

        // Pop the RHS into temporary register
        INSERT_OPERATION(   LOADIMM , 1                 , TEMPORARY     );
        INSERT_OPERATION(   SUB     , STACK_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   LOAD    , STACK_POINTER     , TEMPORARY     );

        // Do the subtraction
        INSERT_OPERATION(   SUB     , ACCUMULATOR       , TEMPORARY     );
    }
    else if(Expr_isCall(expr)) {

        // For each argument expression, evaluate and push the result on the
        // stack
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {

            // Evaluate the idxth argument expression
            INSERT_EXPR_CODE(   Call_arg(expr, idx)                         );

            // Push the result on the stack
            INSERT_OPERATION(   STORE   , ACCUMULATOR       , STACK_POINTER );
            INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
            INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );
        }

        // Add zeros to the stack for the non-argument variables in the callee

        // Push the current frame pointer on the stack
        INSERT_OPERATION(   STORE   , FRAME_POINTER     , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Push the current return address on the stack
        INSERT_OPERATION(   STORE   , RETURN_ADDRESS    , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Set the frame pointer for the callee
        INSERT_OPERATION(           ,                   ,               );

        // Jump to the function
        INSERT_OPERATION(   JUMP    ,                   , 0             );

        // Restore the return address
        INSERT_OPERATION(           ,                   ,               );
        INSERT_OPERATION(           ,                   ,               );
        INSERT_OPERATION(           ,                   ,               );

        // Restore the frame pointer
        INSERT_OPERATION(           ,                   ,               );
        INSERT_OPERATION(           ,                   ,               );
        INSERT_OPERATION(           ,                   ,               );

        // Pop the arguments and variables off the stack
    }
    else if(Expr_isVar(expr)) {

        // Calculate the address from which to load
        INSERT_OPERATION(   LOADIMM , Var_name(expr)    , TEMPORARY     );
        INSERT_OPERATION(   ADD     , TEMPORARY         , FRAME_POINTER );

        // Perform the load operation with the calculated address
        INSERT_OPERATION(   LOAD    , TEMPORARY         , ACCUMULATOR   );
    }
    else {
        ERROR("Expr type not recognised.");
    }

    RETURN_GENERATED_CODE;
}
