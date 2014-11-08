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
    if(Expr_isVar(expr)) {
        // Nothing to do
    }
    else {
        puts("syntax.c/Expr_free(): Expr type not recognised");
        exit(EXIT_FAILURE);
    }
    free(expr);
}
