%{

#include "util.h"

%}

%union {
    Token token;
    Prog *prog;
    Func *func;
    Comm *comm;
    Expr *expr;
}

%token <token> OpenBr
%token <token> ClosBr
%token <token> OpenCr
%token <token> ClosCr
%token <token> Comma
%token <token> While
%token <token> Assign
%token <token> Semi
%token <token> Return
%token <token> Add
%token <token> Sub
%token <token> Name
%token <token> Int

%%

%%

int this_is_the_user_code_section() { return 0; }
