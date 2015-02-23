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

InterpretResult *interpret_Prog(Prog *prog, int *args) {
    return interpret_Func(prog, Prog_func(prog, 0), args);
}

InterpretResult *interpret_Func(Prog *prog, Func *func, int *args) {

    int *store = challoc(sizeof(int) * Prog_next_name(prog));

    // Set initial store values to zero
    int idx;
    for(idx = 0; idx < Prog_next_name(prog); idx++) {
        store[idx] = 0;
    }

    // Initialise the arguments to their given values in the store
    for(idx = 0; idx < Func_num_args(func); idx++) {
        store[Func_arg(func, idx)] = args[idx];
    }

    InterpretResult *res = interpret_Comm(prog, Func_body(func), store);

    free(store);

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

        InterpretResult *comm_res = NULL;

        while(true) {

            InterpretResult *guard_res =
                    interpret_Expr(prog, While_guard(comm), store);

            if((guard_res->type != iSuccess) || (comm_res == NULL)) {
                return guard_res;
            }
            else if((guard_res->result == 0 && comm_res != NULL) ||
                    (guard_res->result != 0 && comm_res->type != iSuccess) ||
                    (guard_res->result != 0 && comm_res->returning)) {

                free(guard_res);
                return comm_res;
            }
            else {
                free(guard_res);
                free(comm_res);

                comm_res = interpret_Comm(prog, While_body(comm), store);
            }
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

        if(res->type != iSuccess || res->returning) {
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

        int *args = challoc(sizeof(int) * Call_num_args(expr));
        bool failed_arg_eval = false;
        InterpretResult *arg_res;

        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {

            arg_res = interpret_Expr(prog, Call_arg(expr, idx), store);
            args[idx] = arg_res->result;

            if(arg_res->type != iSuccess) {
                failed_arg_eval = true;
                break;
            }

            free(arg_res);
        }

        if(failed_arg_eval) {
            free(args);
            return arg_res;
        }

        return interpret_Func(prog, callee, args);
    }
    else if(Expr_isVar(expr)) {
        return InterpretSuccess_init(store[Var_name(expr)]);
    }
    else {
        ERROR("Expr type not recognised.");
    }
}
