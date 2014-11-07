#ifndef util
#include "util.h"
#endif // util

#ifndef syntax
#define syntax

/*
 * Programs
 */

typedef struct Prog Prog;
struct Prog {
    int num_funcs;
    struct Func *funcs[];
};

/*
 * Functions
 */

typedef struct Func Func;
struct Func {
    int name;
    int args;
    struct Comm *body;
};

/*
 * Commands
 */

#define Comm_isWhile(comm) comm->type == commWhile
#define Comm_isAssign(comm) comm->type == commAssign
#define Comm_isComp(comm) comm->type == commComp
#define Comm_isReturn(comm) comm->type == commReturn

#define While_guard(whil) whil->expr
#define While_body(whil) whil->comm1

#define Assign_name(assign) assign->name
#define Assign_expr(assign) assign->expr

#define Comp_fst(comp) comp->comm1
#define Comp_snd(comp) comp->comm2

#define Return_expr(retur) retur->expr

typedef enum { commWhile, commAssign, commComp, commReturn } CommType;

typedef struct Comm Comm;
struct Comm {
    CommType type;
    struct Expr *expr;
    struct Comm *comm1;
    struct Comm *comm2;
    int name;
};

/*
 * Expressions
 */

typedef enum { exprInt, exprAdd, exprSub, exprMul, exprCall } ExprType;

typedef struct Expr Expr;
struct Expr {
    ExprType type;
    int value;
    struct Expr *expr1;
    struct Expr *expr2;
    int name;
    int num_args;
};

Comm *While_init(Expr *guard, Comm* body);
Comm *Assign_init(int name, Expr* expr);
Comm *Comp_init(Comm *comm1, Comm *comm2);
Comm *Return_init(Expr *expr);

#endif // syntax
