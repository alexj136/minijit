#include <stdlib.h>
#include <malloc.h>
#include "syntax.h"
#include "util.h"

/*
 * Programs
 */

Prog *Prog_init(int num_funcs, Func **funcs) {
    Prog *prog = challoc(sizeof(Prog));
    prog->num_funcs = num_funcs;
    prog->funcs = funcs;
    return prog;
}

void Prog_print(Prog *prog, int indent) {
    put_indent(indent);
    printf("PROG: FUNCS=%d:\n", Prog_num_funcs(prog));
    int idx;
    for(idx = 0; idx < Prog_num_funcs(prog); idx++) {
        Func_print(Prog_func(prog, idx), indent + 1);
    }
}

void Prog_free(Prog *prog) {
    int idx;
    for(idx = 0; idx < Prog_num_funcs(prog); idx++) {
        Func_free(Prog_func(prog, idx));
    }
    free(prog);
}

/*
 * Functions
 */

Func *Func_init(int name, int num_args, Comm *body) {
    Func *func = challoc(sizeof(Func));
    func->name = name;
    func->num_args = num_args;
    func->body = body;
    return func;
}

void Func_print(Func *func, int indent) {
    put_indent(indent);
    printf("FUNC: NO=%d, ARGC=%d:\n", Func_name(func), Func_num_args(func));
    Comm_print(Func_body(func), indent + 1);
}

void Func_free(Func *func) {
    Comm_free(Func_body(func));
    free(func);
}

/*
 * Commands
 */

Comm *While_init(Expr *guard, Comm* body) {
    Comm *whil = challoc(sizeof(Comm));
    whil->type = commWhile;
    While_guard(whil) = guard;
    While_body(whil) = body;
    return whil;
}

Comm *Assign_init(int name, Expr* expr) {
    Comm *assign = challoc(sizeof(Comm));
    assign->type = commAssign;
    Assign_name(assign) = name;
    Assign_expr(assign) = expr;
    return assign;
}

Comm *Comp_init(Comm *fst, Comm *snd) {
    Comm *comp = challoc(sizeof(Comm));
    comp->type = commComp;
    Comp_fst(comp) = fst;
    Comp_snd(comp) = snd;
    return comp;
}

Comm *Return_init(Expr *expr) {
    Comm *retur = challoc(sizeof(Comm));
    retur->type = commReturn;
    Return_expr(retur) = expr;
    return retur;
}

void Comm_print(Comm *comm, int indent) {
    put_indent(indent);
    if(Comm_isWhile(comm)) {
        puts("WHILE:");
        Expr_print(While_guard(comm), indent + 1);
        Comm_print(While_body(comm), indent + 1);
    }
    else if(Comm_isAssign(comm)) {
        printf("ASSIGN: ID=%d:\n", Assign_name(comm));
        Expr_print(Assign_expr(comm), indent + 1);
    }
    else if(Comm_isComp(comm)) {
        puts("COMPOSITION:");
        Comm_print(Comp_fst(comm), indent + 1);
        Comm_print(Comp_snd(comm), indent + 1);
    }
    else if(Comm_isReturn(comm)) {
        puts("RETURN:");
        Expr_print(Return_expr(comm), indent + 1);
    }
    else {
        puts("syntax.c/Comm_print(): Comm type not recognised");
        exit(EXIT_FAILURE);
    }
}

void Comm_free(Comm *comm) {
    if(Comm_isWhile(comm)) {
        Expr_free(While_guard(comm));
        Comm_free(While_body(comm));
    }
    else if(Comm_isAssign(comm)) {
        Expr_free(Assign_expr(comm));
    }
    else if(Comm_isComp(comm)) {
        Comm_free(Comp_fst(comm));
        Comm_free(Comp_snd(comm));
    }
    else if(Comm_isReturn(comm)) {
        Expr_free(Return_expr(comm));
    }
    else {
        puts("syntax.c/Comm_free(): Comm type not recognised");
        exit(EXIT_FAILURE);
    }
    free(comm);
}

/*
 * Expressions
 */

Expr *Int_init(int value) {
    Expr *eint = challoc(sizeof(Expr));
    eint->type = exprInt;
    Int_value(eint) = value;
    return eint;
}

Expr *Add_init(Expr *lhs, Expr *rhs) {
    Expr *add = challoc(sizeof(Expr));
    add->type = exprAdd;
    Add_lhs(add) = lhs;
    Add_rhs(add) = rhs;
    return add;
}

Expr *Sub_init(Expr *lhs, Expr *rhs) {
    Expr *sub = challoc(sizeof(Expr));
    sub->type = exprSub;
    Sub_lhs(sub) = lhs;
    Sub_rhs(sub) = rhs;
    return sub;
}

Expr *Call_init(int name, int num_args, Expr **args) {
    Expr *call = challoc(sizeof(Expr));
    call->type = exprCall;
    Call_name(call) = name;
    Call_num_args(call) = num_args;
    Call_args(call) = args;
    return call;
}

Expr *Var_init(int name) {
    Expr *var = challoc(sizeof(Expr));
    var->type = exprVar;
    Var_name(var) = name;
    return var;
}

void Expr_print(Expr *expr, int indent) {
    put_indent(indent);
    if(Expr_isInt(expr)) {
        printf("LITERAL: VALUE=%d\n", Int_value(expr));
    }
    else if(Expr_isAdd(expr)) {
        puts("ADDITION:");
        Expr_print(Add_lhs(expr), indent + 1);
        Expr_print(Add_rhs(expr), indent + 1);
    }
    else if(Expr_isSub(expr)) {
        puts("SUBTRACTION:");
        Expr_print(Sub_lhs(expr), indent + 1);
        Expr_print(Sub_rhs(expr), indent + 1);
    }
    else if(Expr_isCall(expr)) {
        printf("CALL: ID=%d, ARGC=%d:\n", Call_name(expr), Call_num_args(expr));
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {
            Expr_print(Call_arg(expr, idx), indent + 1);
        }
    }
    else if(Expr_isVar(expr)) {
        printf("VARIABLE: ID=%d\n", Var_name(expr));
    }
    else {
        puts("syntax.c/Expr_print(): Expr type not recognised");
        exit(EXIT_FAILURE);
    }
}

void Expr_free(Expr *expr) {
    if(Expr_isInt(expr)) {
        // Nothing to do
    }
    else if(Expr_isAdd(expr)) {
        Expr_free(Add_lhs(expr));
        Expr_free(Add_rhs(expr));
    }
    else if(Expr_isSub(expr)) {
        Expr_free(Sub_lhs(expr));
        Expr_free(Sub_rhs(expr));
    }
    else if(Expr_isCall(expr)) {
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {
            Expr_free(Call_arg(expr, idx));
        }
    }
    else if(Expr_isVar(expr)) {
        // Nothing to do
    }
    else {
        puts("syntax.c/Expr_free(): Expr type not recognised");
        exit(EXIT_FAILURE);
    }
    free(expr);
}
