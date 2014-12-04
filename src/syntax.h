#ifndef util
#include "util.h"
#endif // util

#ifndef syntax
#define syntax

/*
 * Programs
 */

#define Prog_num_funcs(prog) FuncVector_size(prog->funcs)
#define Prog_funcs(prog) prog->funcs
#define Prog_func(prog, num) FuncVector_get(prog->funcs, num)

typedef struct Prog Prog;
struct Prog {
    struct FuncVector *funcs;
};

/*
 * Functions
 */

#define Func_name(func) func->name
#define Func_num_args(func) func->num_args
#define Func_body(func) func->body

typedef struct Func Func;
struct Func {
    int name;
    int num_args;
    struct Comm *body;
};

FORWARD_DECLARE_VECTORABLE(Func)

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

#define Expr_isInt(expr) expr->type == exprInt
#define Expr_isAdd(expr) expr->type == exprAdd
#define Expr_isSub(expr) expr->type == exprSub
#define Expr_isCall(expr) expr->type == exprCall
#define Expr_isVar(expr) expr->type == exprVar

#define Int_value(eint) eint->value

#define Add_lhs(add) add->expr1
#define Add_rhs(add) add->expr2

#define Sub_lhs(sub) sub->expr1
#define Sub_rhs(sub) sub->expr2

#define Call_name(call) call->name
#define Call_num_args(call) call->num_args
#define Call_args(call) call->args
#define Call_arg(call, num) call->args[num]

#define Var_name(var) var->name

typedef enum { exprInt, exprAdd, exprSub, exprCall, exprVar } ExprType;

typedef struct Expr Expr;
struct Expr {
    ExprType type;
    int value;
    struct Expr *expr1;
    struct Expr *expr2;
    int name;
    int num_args;
    struct Expr **args;
};

/*
 * Syntax functions
 */

Prog *Prog_init(FuncVector *funcs);
void Prog_print(Prog *prog, int indent);
void Prog_free(Prog *prog);

Func *Func_init(int name, int num_args, Comm *body);
void Func_print(Func *func, int indent);
void Func_free(Func *func);

Comm *While_init(Expr *guard, Comm* body);
Comm *Assign_init(int name, Expr* expr);
Comm *Comp_init(Comm *fst, Comm *snd);
Comm *Return_init(Expr *expr);
void Comm_print(Comm *comm, int indent);
void Comm_free(Comm *comm);

Expr *Int_init(int value);
Expr *Add_init(Expr *lhs, Expr *rhs);
Expr *Sub_init(Expr *lhs, Expr *rhs);
Expr *Call_init(int name, int num_args, Expr **args);
Expr *Var_init(int name);
void Expr_print(Expr *expr, int indent);
void Expr_free(Expr *expr);

#endif // syntax
