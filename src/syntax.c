#include <stdlib.h>
#include <malloc.h>
#include "syntax.h"
#include "util.h"

/*
 * Programs
 */

Prog *Prog_init(FuncVector *funcs, charVector *name_map, int next_name) {
    Prog *prog = challoc(sizeof(Prog));
    prog->name_map = name_map;
    prog->next_name = next_name;
    prog->funcs = funcs;
    return prog;
}

/*
 * Retreive a function from a program based on the function's name. Returns null
 * if there is no function by that name.
 */
Func *Prog_lookup_Func(Prog *prog, int name) {

    int  idx   = 0;
    bool found = false;

    while((!found) && idx < Prog_num_funcs(prog)) {

        if(name == Func_name(Prog_func(prog, idx))) {
            found = true;
        }
        else {
            idx++;
        }

    }

    if(found) {
        return Prog_func(prog, idx);
    }
    else {
        return NULL;
    }
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
    int idx;
    for(idx = 0; idx < Prog_num_funcs(prog); idx++) {
        Func_print(Prog_func(prog, idx), indent, prog->name_map);
    }
}

void Prog_free(Prog *prog) {
    FuncVector_free_elems(prog->funcs);
    charVector_free_elems(prog->name_map);
    free(prog);
}

/*
 * Functions
 */

DEFINE_VECTORABLE(Func)

Func *Func_init(int name, IntRefVector *args, Comm *body) {
    return Func_init_pos(name, args, body, -1, -1);
}

Func *Func_init_pos(int name, IntRefVector *args, Comm *body, int src_line_no,
        int src_char_no) {

    Func *func = challoc(sizeof(Func));
    func->name = name;
    func->args = args;
    func->body = body;
    func->src_line_no = src_line_no;
    func->src_char_no = src_char_no;
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

void Func_print(Func *func, int indent, charVector *name_map) {
    put_indent(indent);
    printf("%s(", charVector_get(name_map, Func_name(func)));
    int idx;
    for(idx = 0; idx < Func_num_args(func); idx++) {
        printf("%s", charVector_get(name_map, Func_arg(func, idx)));
        if(idx + 1 < Func_num_args(func)) { printf(", "); }
    }
    printf(") {\n");
    Comm_print(Func_body(func), indent + 1, name_map);
    printf("\n");
    put_indent(indent);
    printf("}\n");
}

void Func_free(Func *func) {
    IntRefVector_free_elems(func->args);
    Comm_free(Func_body(func));
    free(func);
}

/*
 * Commands
 */

Comm *Comm_init(CommType type, Expr *expr, Comm *comm1, Comm *comm2, int name,
        int src_line_no, int src_char_no) {

    Comm *new = challoc(sizeof(Comm));
    new->type = type;
    new->expr = expr;
    new->comm1 = comm1;
    new->comm2 = comm2;
    new->name = name;
    new->src_line_no = src_line_no;
    new->src_char_no = src_char_no;
    return new;
}

Comm *While_init(Expr *guard, Comm* body) {
    return Comm_init(commWhile, guard, body, NULL, -1, -1, -1);
}

Comm *Assign_init(int name, Expr* expr) {
    return Comm_init(commAssign, expr, NULL, NULL, name, -1, -1);
}

Comm *Comp_init(Comm *fst, Comm *snd) {
    return Comm_init(commComp, NULL, fst, snd, -1, -1, -1);
}

Comm *Return_init(Expr *expr) {
    return Comm_init(commReturn, expr, NULL, NULL, -1, -1, -1);
}

Comm *While_init_pos(Expr *guard, Comm* body, int src_line_no,
        int src_char_no) {

    return Comm_init(commWhile, guard, body, NULL, -1, src_line_no,
            src_char_no);
}

Comm *Assign_init_pos(int name, Expr* expr, int src_line_no, int src_char_no) {
    return Comm_init(commAssign, expr, NULL, NULL, name, src_line_no,
            src_char_no);
}

Comm *Comp_init_pos(Comm *fst, Comm *snd, int src_line_no, int src_char_no) {
    return Comm_init(commComp, NULL, fst, snd, -1, src_line_no, src_char_no);
}

Comm *Return_init_pos(Expr *expr, int src_line_no, int src_char_no) {
    return Comm_init(commReturn, expr, NULL, NULL, -1, src_line_no,
            src_char_no);
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

void Comm_print(Comm *comm, int indent, charVector *name_map) {
    if(Comm_isWhile(comm)) {
        put_indent(indent);
        printf("while ");
        Expr_print(While_guard(comm), name_map);
        printf(" do {\n");
        Comm_print(While_body(comm), indent + 1, name_map);
        printf("\n");
        put_indent(indent);
        printf("}");
    }
    else if(Comm_isAssign(comm)) {
        put_indent(indent);
        printf("%s := ", charVector_get(name_map, Assign_name(comm)));
        Expr_print(Assign_expr(comm), name_map);
    }
    else if(Comm_isComp(comm)) {
        Comm_print(Comp_fst(comm), indent, name_map);
        printf(";\n");
        Comm_print(Comp_snd(comm), indent, name_map);
    }
    else if(Comm_isReturn(comm)) {
        put_indent(indent);
        printf("return ");
        Expr_print(Return_expr(comm), name_map);
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
        ExprVector *args, int src_line_no, int src_char_no) {

    Expr *expr = challoc(sizeof(Expr));
    expr->type = type;
    expr->num = num;
    expr->expr1 = expr1;
    expr->expr2 = expr2;
    expr->args = args;
    expr->src_line_no = src_line_no;
    expr->src_char_no = src_char_no;
    return expr;
}

Expr *Int_init(int value) {
    return Expr_init(exprInt, value, NULL, NULL, NULL, -1, -1);
}

Expr *Add_init(Expr *lhs, Expr *rhs) {
    return Expr_init(exprAdd, -1, lhs, rhs, NULL, -1, -1);
}

Expr *Sub_init(Expr *lhs, Expr *rhs) {
    return Expr_init(exprSub, -1, lhs, rhs, NULL, -1, -1);
}

Expr *Call_init(int name, ExprVector *args) {
    return Expr_init(exprCall, name, NULL, NULL, args, -1, -1);
}

Expr *Var_init(int name) {
    return Expr_init(exprVar, name, NULL, NULL, NULL, -1, -1);
}

Expr *Int_init_pos(int value, int src_line_no, int src_char_no) {
    return Expr_init(exprInt, value, NULL, NULL, NULL, src_line_no,
            src_char_no);
}

Expr *Add_init_pos(Expr *lhs, Expr *rhs, int src_line_no, int src_char_no) {
    return Expr_init(exprAdd, -1, lhs, rhs, NULL, src_line_no, src_char_no);
}

Expr *Sub_init_pos(Expr *lhs, Expr *rhs, int src_line_no, int src_char_no) {
    return Expr_init(exprSub, -1, lhs, rhs, NULL, src_line_no, src_char_no);
}

Expr *Call_init_pos(int name, ExprVector *args, int src_line_no,
        int src_char_no) {

    return Expr_init(exprCall, name, NULL, NULL, args, src_line_no,
            src_char_no);
}

Expr *Var_init_pos(int name, int src_line_no, int src_char_no) {
    return Expr_init(exprVar, name, NULL, NULL, NULL, src_line_no, src_char_no);
}

bool Expr_eq(Expr *e1, Expr *e2) {
    if((!e1) || (!e2))  { return e1 == e2; } // Null check
    else {
        return ((e1->type) == (e2->type)) &&
                ((e1->num) == (e2->num)) &&
                Expr_eq(e1->expr1, e2->expr1) &&
                Expr_eq(e1->expr2, e2->expr2) &&
                (((e1->args) && (e2->args)) ?
                        ExprVector_eq(e1->args, e2->args) :
                        ((e1->args) == (e2->args)));
    }
}

void Expr_print(Expr *expr, charVector *name_map) {
    if(Expr_isInt(expr)) {
        printf("%d", Int_value(expr));
    }
    else if(Expr_isAdd(expr)) {
        Expr_print(Add_lhs(expr), name_map);
        printf(" + ");
        Expr_print(Add_rhs(expr), name_map);
    }
    else if(Expr_isSub(expr)) {
        Expr_print(Sub_lhs(expr), name_map);
        printf(" - ");
        Expr_print(Sub_rhs(expr), name_map);
    }
    else if(Expr_isCall(expr)) {
        printf("%s(", charVector_get(name_map, Call_name(expr)));
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {
            Expr_print(Call_arg(expr, idx), name_map);
            if(idx + 1 < Call_num_args(expr)) { printf(", "); }
        }
        printf(")");
    }
    else if(Expr_isVar(expr)) {
        printf("%s", charVector_get(name_map, Var_name(expr)));
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
