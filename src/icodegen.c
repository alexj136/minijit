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
    ICodeOperationVector *ops = ICodeOperationVector_init()

#define INSERT_OPERATION(___opc, ___arg1, ___arg2) \
    ICodeOperationVector_append(ops, \
            (ICodeOperation_init(___opc, ___arg1, ___arg2)))

#define INSERT_FUNC_CODE(___func) ___INSERT_CODE(Func, ___func)
#define INSERT_COMM_CODE(___comm) ___INSERT_CODE(Comm, ___comm)
#define INSERT_EXPR_CODE(___expr) ___INSERT_CODE(Expr, ___expr)
#define ___INSERT_CODE(___ty, ___ast) \
    do { \
        ICodeOperationVector *___code = \
                icodegen_##___ty(prog, ___ast, next_label, func_labels); \
        ICodeOperationVector_append_all(ops, ___code); \
        ICodeOperationVector_free(___code); \
    } while(0)

#define RETURN_GENERATED_CODE \
    return ops

/*
 * Code generation functions - (accumulator strategy)
 */

/*
 * The generated code expects the stack to be preconfigured as would be required
 * by the main function. The arguments should be on top of the stack, with
 * zeroed spaces prepared for other variables underneath them.
 */
ICodeOperationVector *icodegen_Prog(Prog *prog, int *next_label) {

    // Allocate space for the map from function names to label names
    int *func_labels = challoc(sizeof(int) * Prog_num_funcs(prog));

    // Populate the map from function names to label names
    int idx;
    for(idx = 0; idx < Prog_num_funcs(prog); idx++) {
        func_labels[idx] = (*next_label)++;
    }

    BEGIN_CODE_GENERATION;

    // Make a call to main
    INSERT_OPERATION(JUMPLINK, func_labels[0], 0);

    // Halt when the call returns
    INSERT_OPERATION(HALT, 0, 0);

    for(idx = 0; idx < Prog_num_funcs(prog); idx++) {
        INSERT_FUNC_CODE(Prog_func(prog, idx));
    }

    RETURN_GENERATED_CODE;
}

ICodeOperationVector *icodegen_Func(Prog *prog, Func *func, int *next_label,
        int *func_labels) {

    // Determine the label to use as the entry label for this function
    int func_label = func_labels[Func_name(func)];

    BEGIN_CODE_GENERATION;

    // Since the caller does all the work of a function call, all we include is
    // an entry label and code for the function body.
    INSERT_OPERATION(LABEL, func_label, 0);
    INSERT_COMM_CODE(Func_body(func));

    RETURN_GENERATED_CODE;
}

ICodeOperationVector *icodegen_Comm(Prog *prog, Comm *comm, int *next_label,
        int *func_labels) {

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
        INSERT_OPERATION(   MOVE    , FRAME_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   SUB     , TEMPORARY         , Assign_name(comm));

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
        INSERT_EXPR_CODE(   Return_expr(comm)                           );

        // Return to caller
        INSERT_OPERATION(   JUMPADDR, RETURN_ADDRESS    , 0             );
    }
    else {
        ERROR("Comm type not recognised.");
    }

    RETURN_GENERATED_CODE;
}

ICodeOperationVector *icodegen_Expr(Prog *prog, Expr *expr, int *next_label,
        int *func_labels) {

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

        // Determine the label number to jump to in the call
        int callee_address = func_labels[Call_name(expr)];
        int num_args       = Call_num_args(expr);
        int num_vars       = Func_num_vars(Prog_func(prog, Call_name(expr)));
        int idx;

        // Push the current frame pointer on the stack
        INSERT_OPERATION(   STORE   , FRAME_POINTER     , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Push the current return address on the stack
        INSERT_OPERATION(   STORE   , RETURN_ADDRESS    , STACK_POINTER );
        INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
        INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );

        // Add zeros to the stack for the non-argument variables in the callee
        for(idx = num_vars - 1; idx >= num_args; idx--) {

            INSERT_OPERATION(   LOADIMM , 0                 , ACCUMULATOR   );
            INSERT_OPERATION(   STORE   , ACCUMULATOR       , STACK_POINTER );
            INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
            INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );
        }

        // For each argument expression, evaluate and push the result on the
        // stack
        for(idx = num_args - 1; idx >= 0; idx--) {

            // Evaluate the idxth argument expression
            INSERT_EXPR_CODE(   Call_arg(expr, idx)                         );

            // Push the result on the stack
            INSERT_OPERATION(   STORE   , ACCUMULATOR       , STACK_POINTER );
            INSERT_OPERATION(   LOADIMM , 1                 , ACCUMULATOR   );
            INSERT_OPERATION(   ADD     , STACK_POINTER     , ACCUMULATOR   );
        }

        // Set the frame pointer for the callee
        INSERT_OPERATION(   MOVE    , STACK_POINTER     , FRAME_POINTER );

        // Jump to the function
        INSERT_OPERATION(   JUMPLINK, callee_address    , 0             );

        // Pop the arguments and variables off the stack
        INSERT_OPERATION(   LOADIMM , num_vars          , TEMPORARY     );
        INSERT_OPERATION(   SUB     , STACK_POINTER     , TEMPORARY     );

        // Restore the return address
        INSERT_OPERATION(   LOADIMM , 1                 , TEMPORARY     );
        INSERT_OPERATION(   SUB     , STACK_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   LOAD    , STACK_POINTER     , RETURN_ADDRESS);

        // Restore the frame pointer
        INSERT_OPERATION(   LOADIMM , 1                 , TEMPORARY     );
        INSERT_OPERATION(   SUB     , STACK_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   LOAD    , STACK_POINTER     , FRAME_POINTER );
    }
    else if(Expr_isVar(expr)) {

        // Calculate the address from which to load
        INSERT_OPERATION(   MOVE    , FRAME_POINTER     , TEMPORARY     );
        INSERT_OPERATION(   SUB     , TEMPORARY         , Var_name(expr));

        // Perform the load operation with the calculated address
        INSERT_OPERATION(   LOAD    , TEMPORARY         , ACCUMULATOR   );
    }
    else {
        ERROR("Expr type not recognised.");
    }

    RETURN_GENERATED_CODE;
}
