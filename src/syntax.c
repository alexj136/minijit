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

Comm *Comm_init(CommType type, Expr *expr, Comm *comm1, Comm *comm2, int name) {
    Comm *new = challoc(sizeof(Comm));
    new->type = type;
    new->expr = expr;
    new->comm1 = comm1;
    new->comm2 = comm2;
    new->name = name;
    return new;
}

Comm *While_init(Expr *guard, Comm* body) {
    return Comm_init(commWhile, guard, body, NULL, -1);
}

Comm *Assign_init(int name, Expr* expr) {
    return Comm_init(commAssign, expr, NULL, NULL, name);
}

Comm *Comp_init(Comm *fst, Comm *snd) {
    return Comm_init(commComp, NULL, fst, snd, -1);
}

Comm *Return_init(Expr *expr) {
    return Comm_init(commReturn, expr, NULL, NULL, -1);
}

bool Comm_eq(Comm *c1, Comm *c2) {
    if((!c1) || (!c2))  { return c1 == c2; } // Null check
    else {
        return ((c1->type) == (c2->type)) &&
                ((c1->name) == (c2->name)) &&
                Expr_eq(c1->expr, c2->expr) &&
                Comm_eq(c1->comm1, c2->comm1) &&
                Comm_eq(c1->comm2, c2->comm2);
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
    if(!comm) { return; } // Null check
    Expr_free(comm->expr);
    Comm_free(comm->comm1);
    Comm_free(comm->comm2);
    free(comm);
}

/*
 * Expressions
 */

DEFINE_VECTORABLE(Expr)

Expr *Expr_init(ExprType type, int num, Expr *expr1, Expr *expr2,
        ExprVector *args) {

    Expr *expr = challoc(sizeof(Expr));
    expr->type = type;
    expr->num = num;
    expr->expr1 = expr1;
    expr->expr2 = expr2;
    expr->args = args;
    return expr;
}

Expr *Int_init(int value) {
    return Expr_init(exprInt, value, NULL, NULL, NULL);
}

Expr *Add_init(Expr *lhs, Expr *rhs) {
    return Expr_init(exprAdd, -1, lhs, rhs, NULL);
}

Expr *Sub_init(Expr *lhs, Expr *rhs) {
    return Expr_init(exprSub, -1, lhs, rhs, NULL);
}

Expr *Call_init(int name, ExprVector *args) {
    return Expr_init(exprCall, name, NULL, NULL, args);
}

Expr *Var_init(int name) {
    return Expr_init(exprVar, name, NULL, NULL, NULL);
}

bool Expr_eq(Expr *e1, Expr *e2) {
    if((!e1) || (!e2))  { return e1 == e2; } // Null check
    else {
        return ((e1->type) == (e2->type)) &&
                ((e1->num) == (e2->num)) &&
                Expr_eq(e1->expr1, e2->expr1) &&
                Expr_eq(e1->expr2, e2->expr2) &&
                ExprVector_eq(e1->args, e2->args);
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
    if(!expr) { return; } // Null check
    Expr_free(expr->expr1);
    Expr_free(expr->expr2);
    ExprVector_free_elems(expr->args);
    free(expr);
}
