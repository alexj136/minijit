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
