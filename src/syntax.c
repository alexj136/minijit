#include <stdlib.h>
#include <malloc.h>
#include "syntax.h"
#include "util.h"

/*
 * Programs
 */

Prog *Prog_init(FuncVector *funcs) {
    Prog *prog = challoc(sizeof(Prog));
    prog->funcs = funcs;
    return prog;
}

/*
 * Syntactic equality - NOT alpha-equivalence. Sensetive to virtually
 * everything, e.g. choice of identifier names, order of declared functions etc.
 */
bool Prog_eq(Prog *p, Prog *q) {
    bool same = true;
    if(Prog_num_funcs(p) != Prog_num_funcs(q)) { same = false; }
    int idx = 0;
    while(same && (idx < Prog_num_funcs(p))) {
        if(!Func_eq(Prog_func(p, idx), Prog_func(q, idx))) { same = false; }
        idx++;
    }
    return same;
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
    FuncVector_free_elems(prog->funcs);
    free(prog);
}

/*
 * Functions
 */

DEFINE_VECTORABLE(Func)

Func *Func_init(int name, IntRefVector *args, Comm *body) {
    Func *func = challoc(sizeof(Func));
    func->name = name;
    func->args = args;
    func->body = body;
    return func;
}

bool Func_eq(Func *f, Func *g) {
    bool same = true;

    if((Func_name(f) != Func_name(g)) ||
            (Func_num_args(f) != Func_num_args(g))) {

        same = false;
    }
    
    int idx = 0;
    while(same && (idx < Func_num_args(f))) {
        if(Func_arg(f, idx) != Func_arg(g, idx)) { same = false; }
        idx++;
    }

    // Extra 'same &&' guard condition prevents us from doing a potentially
    // expensive call to Comm_eq() when we already know the functions are
    // different.
    if(same && (!Comm_eq(Func_body(f), Func_body(g)))) { same = false; }

    return same;
}

void Func_print(Func *func, int indent) {
    put_indent(indent);
    printf("FUNC: NO=%d, ARGC=%d, ARGIDS=", Func_name(func),
            Func_num_args(func));
    int idx;
    for(idx = 0; idx < Func_num_args(func); idx++) {
        printf("%d ", Func_arg(func, idx));
    }
    printf(":\n");
    Comm_print(Func_body(func), indent + 1);
}

void Func_free(Func *func) {
    IntRefVector_free_elems(func->args);
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

bool Comm_eq(Comm *c1, Comm *c2) {
    if(Comm_isWhile(c1) && Comm_isWhile(c2)) {
        return Expr_eq(While_guard(c1), While_guard(c2)) &&
                Comm_eq(While_body(c1), While_body(c2));
    }
    else if(Comm_isAssign(c1) && Comm_isAssign(c2)) {
        return (Assign_name(c1) == Assign_name(c2)) &&
                Expr_eq(Assign_expr(c1), Assign_expr(c2));
    }
    else if(Comm_isComp(c1) && Comm_isComp(c2)) {
        return Comm_eq(Comp_fst(c1), Comp_fst(c2)) &&
                Comm_eq(Comp_snd(c1), Comp_snd(c2));
    }
    else if(Comm_isReturn(c1) && Comm_isReturn(c2)) {
        return Expr_eq(Return_expr(c1), Return_expr(c2));
    }
    else {
        return false;
    }
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
        ERROR("Comm type not recognised.");
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
        ERROR("Comm type not recognised.");
    }
    free(comm);
}

/*
 * Expressions
 */

DEFINE_VECTORABLE(Expr)

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

Expr *Call_init(int name, ExprVector *args) {
    Expr *call = challoc(sizeof(Expr));
    call->type = exprCall;
    Call_name(call) = name;
    Call_args(call) = args;
    return call;
}

Expr *Var_init(int name) {
    Expr *var = challoc(sizeof(Expr));
    var->type = exprVar;
    Var_name(var) = name;
    return var;
}

bool Expr_eq(Expr *e1, Expr *e2) {
    if(Expr_isInt(e1) && Expr_isInt(e2)) {
        return Int_value(e1) == Int_value(e2);
    }
    else if(Expr_isAdd(e1) && Expr_isAdd(e2)) {
        return Expr_eq(Add_lhs(e1), Add_lhs(e2)) &&
                Expr_eq(Add_rhs(e1), Add_rhs(e2));
    }
    else if(Expr_isSub(e1) && Expr_isSub(e2)) {
        return Expr_eq(Sub_lhs(e1), Sub_lhs(e2)) &&
                Expr_eq(Sub_rhs(e1), Sub_rhs(e2));
    }
    else if(Expr_isCall(e1) && Expr_isCall(e2)) {
        bool same = true;
        if((Call_name(e1) != Call_name(e2)) &&
                (Call_num_args(e1) != Call_num_args(e2))) { same = false; }
        int idx = 0;
        while(same && (idx < Call_num_args(e1))) {
            if(!Expr_eq(Call_arg(e1, idx), Call_arg(e2, idx))) {
                same = false;
            }
            idx++;
        }
        return same;
    }
    else if(Expr_isVar(e1) && Expr_isVar(e2)) {
        return Var_name(e1) == Var_name(e2);
    }
    else {
        return false;
    }
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
        ERROR("Expr type not recognised.");
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
        ExprVector_free_elems(Call_args(expr));
    }
    else if(Expr_isVar(expr)) {
        // Nothing to do
    }
    else {
        ERROR("Expr type not recognised.");
    }
    free(expr);
}
