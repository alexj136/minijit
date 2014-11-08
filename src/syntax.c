#include <malloc.h>
#include "syntax.h"
#include "util.h"

Comm *While_init(Expr *guard, Comm* body) {
    Comm *whil = challoc(sizeof(Comm));
    whil->type = commWhile;
    whil->expr = guard;
    whil->comm1 = body;
    return whil;
}

Comm *Assign_init(int name, Expr* expr) {
    Comm *assign = challoc(sizeof(Comm));
    assign->type = commAssign;
    assign->name = name;
    assign->expr = expr;
    return assign;
}

Comm *Comp_init(Comm *comm1, Comm *comm2) {
    Comm *comp = challoc(sizeof(Comm));
    comp->type = commComp;
    comp->comm1 = comm1;
    comp->comm2 = comm2;
    return comp;
}

Comm *Return_init(Expr *expr) {
    Comm *retur = challoc(sizeof(Comm));
    retur->type = commReturn;
    retur->expr = expr;
    return retur;
}

Expr *Int_init(int value) {
    Expr *eint = challoc(sizeof(Expr));
    eint->type = exprInt;
    eint->value = value;
    return eint;
}

Expr *Add_init(Expr *lhs, Expr *rhs) {
    Expr *add = challoc(sizeof(Expr));
    add->type = exprAdd;
    add->expr1 = lhs;
    add->expr2 = rhs;
    return add;
}

Expr *Sub_init(Expr *lhs, Expr *rhs) {
    Expr *sub = challoc(sizeof(Expr));
    sub->type = exprSub;
    sub->expr1 = lhs;
    sub->expr2 = rhs;
    return sub;
}

Expr *Call_init(int name, int num_args, Expr **args) {
    Expr *call = challoc(sizeof(Expr));
    call->type = exprCall;
    call->name = name;
    call->num_args = num_args;
    call->args = args;
    return call;
}
