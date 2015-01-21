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
#define Func_num_args(func) IntRefVector_size(func->args)
#define Func_arg(func, num) IntRef_value(IntRefVector_get(func->args, num))
#define Func_body(func) func->body

typedef struct Func Func;
struct Func {
    int name;
    struct IntRefVector *args;
    struct Comm *body;
    int src_line_no;
    int src_char_no;
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
    int src_line_no;
    int src_char_no;
};

/*
 * Expressions
 */

#define Expr_isInt(expr) expr->type == exprInt
#define Expr_isAdd(expr) expr->type == exprAdd
#define Expr_isSub(expr) expr->type == exprSub
#define Expr_isCall(expr) expr->type == exprCall
#define Expr_isVar(expr) expr->type == exprVar

#define Int_value(eint) eint->num

#define Add_lhs(add) add->expr1
#define Add_rhs(add) add->expr2

#define Sub_lhs(sub) sub->expr1
#define Sub_rhs(sub) sub->expr2

#define Call_name(call) call->num
#define Call_num_args(call) ExprVector_size(call->args)
#define Call_args(call) call->args
#define Call_arg(call, num) ExprVector_get(call->args, num)

#define Var_name(var) var->num

typedef enum { exprInt, exprAdd, exprSub, exprCall, exprVar } ExprType;

typedef struct Expr Expr;
struct Expr {
    ExprType type;
    int num;
    struct Expr *expr1;
    struct Expr *expr2;
    struct ExprVector *args;
    int src_line_no;
    int src_char_no;
};

FORWARD_DECLARE_VECTORABLE(Expr);

/*
 * Syntax functions
 */

Prog *Prog_init(FuncVector *funcs);
bool Prog_eq(Prog *p, Prog *q);
void Prog_print(Prog *prog, int indent, charVector *name_map);
void Prog_free(Prog *prog);

Func *Func_init(int name, IntRefVector *args, Comm *body, int src_line_no,
        int src_char_no);
bool Func_eq(Func *f, Func *g);
void Func_print(Func *func, int indent, charVector *name_map);
void Func_free(Func *func);

Comm *While_init(Expr *guard, Comm* body);
Comm *Assign_init(int name, Expr* expr);
Comm *Comp_init(Comm *fst, Comm *snd);
Comm *Return_init(Expr *expr);
Comm *While_init_pos(Expr *guard, Comm* body, int src_line_no, int src_char_no);
Comm *Assign_init_pos(int name, Expr* expr, int src_line_no, int src_char_no);
Comm *Comp_init_pos(Comm *fst, Comm *snd, int src_line_no, int src_char_no);
Comm *Return_init_pos(Expr *expr, int src_line_no, int src_char_no);
bool Comm_eq(Comm *c1, Comm *c2);
void Comm_print(Comm *comm, int indent, charVector *name_map);
void Comm_free(Comm *comm);

Expr *Int_init(int value);
Expr *Add_init(Expr *lhs, Expr *rhs);
Expr *Sub_init(Expr *lhs, Expr *rhs);
Expr *Call_init(int name, ExprVector *args);
Expr *Var_init(int name);
Expr *Int_init_pos(int value, int src_line_no, int src_char_no);
Expr *Add_init_pos(Expr *lhs, Expr *rhs, int src_line_no, int src_char_no);
Expr *Sub_init_pos(Expr *lhs, Expr *rhs, int src_line_no, int src_char_no);
Expr *Call_init_pos(int name, ExprVector *args, int src_line_no,
        int src_char_no);
Expr *Var_init_pos(int name, int src_line_no, int src_char_no);
bool Expr_eq(Expr *e1, Expr *e2);
void Expr_print(Expr *expr, charVector *name_map);
void Expr_free(Expr *expr);

#endif // syntax
