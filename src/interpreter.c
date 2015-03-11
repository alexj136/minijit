#include <stdio.h>
#include <stdlib.h>
#include "syntax.h"
#include "util.h"
#include "interpreter.h"

InterpretResult *InterpretResult_init(interpretResultType type, int result,
        bool returning, int line_no, int char_no) {

    InterpretResult *ir = challoc(sizeof(InterpretResult));
    ir->type      = type;
    ir->result    = result;
    ir->returning = returning;
    ir->line_no   = line_no;
    ir->char_no   = char_no;
    return ir;
}

InterpretResult *InterpretSuccess_init(int result) {
    return InterpretResult_init(iSuccess, result, false, -1, -1);
}

InterpretResult *InterpretFailIncorrectNumArgs_init(int line_no, int char_no) {
    return InterpretResult_init(iFailIncorrectNumArgs, 0, false, line_no,
            char_no);
}

InterpretResult *InterpretFailFunctionNotFound_init(int line_no, int char_no) {
    return InterpretResult_init(iFailFunctionNotFound, 0, false, line_no,
            char_no);
}

InterpretResult *InterpretFailEndWithoutReturn_init(int line_no, int char_no) {
    return InterpretResult_init(iFailEndWithoutReturn, 0, false, line_no,
            char_no);
}

/*
 * Interpreter function definitions
 */

InterpretResult *interpret_Prog(Prog *prog, IntRefVector *args) {

    // Create a Call Expr to call main
    ExprVector *arg_exprs = ExprVector_init();
    int idx;
    for(idx = 0; idx < IntRefVector_size(args); idx++) {
        ExprVector_append(arg_exprs,
                Int_init(IntRef_value(IntRefVector_get(args, idx))));
    }
    Expr *main_call = Call_init(0, arg_exprs);

    // Run the call
    InterpretResult *result = interpret_Expr(prog, main_call, NULL);

    // Free the call
    Expr_free(main_call);

    // Return the result
    return result;
}

InterpretResult *interpret_Func(Prog *prog, Func *func, int *store) {

    // The store is created for us in the interpret_Expr() Call case. Run the
    // command with the store, check for errors, and return.

    InterpretResult *res = interpret_Comm(prog, Func_body(func), store);

    if(res->type == iSuccess && res->returning == false) {
        free(res);
        return InterpretFailEndWithoutReturn_init(Func_src_line_no(func),
                Func_src_char_no(func));
    }
    else {
        return res;
    }
}

InterpretResult *interpret_Comm(Prog *prog, Comm *comm, int *store) {
    if(Comm_isWhile(comm)) {

        InterpretResult *guard_res =
                interpret_Expr(prog, While_guard(comm), store);

        if((guard_res->type != iSuccess) || (guard_res->result < 1)) {
            return guard_res;
        }

        while(true) {

            InterpretResult *comm_res =
                    interpret_Comm(prog, While_body(comm), store);

            if((comm_res->type != iSuccess) ||(comm_res->returning == true)) {
                free(guard_res);
                return comm_res;
            }

            free(guard_res);
            guard_res = interpret_Expr(prog, While_guard(comm), store);

            if(guard_res->type != iSuccess) {
                free(comm_res);
                return guard_res;
            }
            else if(guard_res->result < 1) {
                free(guard_res);
                return comm_res;
            }

            free(comm_res);
        }
    }
    else if(Comm_isAssign(comm) || Comm_isReturn(comm)) {

        InterpretResult *res = interpret_Expr(prog, Assign_expr(comm), store);

        if(res->type != iSuccess) {
            return res;
        }

        if(Comm_isAssign(comm)) {
            store[Assign_name(comm)] = res->result;
        }
        else /* if(Comm_isReturn(comm)) */ {
            res->returning = true;
        }

        return res;
    }
    else if(Comm_isComp(comm)) {

        InterpretResult *res = interpret_Comm(prog, Comp_fst(comm), store);

        if((res->type != iSuccess) || res->returning) {
            return res;
        }

        free(res);

        return interpret_Comm(prog, Comp_snd(comm), store);
    }
    else {
        ERROR("Comm type not recognised.");
    }
}

InterpretResult *interpret_Expr(Prog *prog, Expr *expr, int *store) {
    if(Expr_isInt(expr)) {
        return InterpretSuccess_init(Int_value(expr));
    }
    else if(Expr_isAdd(expr) || Expr_isSub(expr)) {
        InterpretResult *lhs;
        InterpretResult *rhs;

        if(Expr_isAdd(expr)) {
            lhs = interpret_Expr(prog, Add_lhs(expr), store);
            rhs = interpret_Expr(prog, Add_rhs(expr), store);
        }
        else /* if(Expr_isSub(expr)) */ {
            lhs = interpret_Expr(prog, Sub_lhs(expr), store);
            rhs = interpret_Expr(prog, Sub_rhs(expr), store);
        }

        if((lhs->type == iSuccess) && (rhs->type == iSuccess)) {
            int lhs_val = lhs->result;
            int rhs_val = rhs->result;

            free(lhs);
            free(rhs);

            return InterpretSuccess_init(
                    lhs_val + (Expr_isSub(expr) ? -rhs_val : rhs_val));
        }

        else if(lhs->type != iSuccess) {
            free(rhs);
            return lhs;
        }

        else /* if(rhs->type != iSuccess) */ {
            free(lhs);
            return rhs;
        }
    }
    else if(Expr_isCall(expr)) {

        Func *callee = Prog_lookup_Func(prog, Call_name(expr));

        if(callee == NULL) {
            return InterpretFailFunctionNotFound_init(Expr_src_line_no(expr),
                    Expr_src_char_no(expr));
        }

        if(Func_num_args(callee) != Call_num_args(expr)) {
            return InterpretFailIncorrectNumArgs_init(Expr_src_line_no(expr),
                    Expr_src_char_no(expr));
        }

        int *callee_store = challoc(sizeof(int) * Func_num_vars(callee));
        bool failed_arg_eval = false;
        InterpretResult *arg_res;

        // Evaluate each argument expression. Stop early if we encounter an
        // error.
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {

            arg_res = interpret_Expr(prog, Call_arg(expr, idx), store);
            callee_store[idx] = arg_res->result;

            if(arg_res->type != iSuccess) {
                failed_arg_eval = true;
                break;
            }

            free(arg_res);
        }

        // If an error was encountered in argument evaluation, clean up and
        // return an error as the result of the entire call
        if(failed_arg_eval) {
            free(callee_store);
            return arg_res;
        }

        // Argument evaluation succeeded, so write zero in place for the initial
        // values of the other variables in the callee's store
        for(idx = Func_num_args(callee); idx < Func_num_vars(callee); idx++) {
            callee_store[idx] = 0;
        }

        // Make the call, free the argument array, and return the call result
        InterpretResult *call_res = interpret_Func(prog, callee, callee_store);
        free(callee_store);
        return call_res;
    }
    else if(Expr_isVar(expr)) {
        return InterpretSuccess_init(store[Var_name(expr)]);
    }
    else {
        ERROR("Expr type not recognised.");
    }
}
