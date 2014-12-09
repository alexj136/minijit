%{

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"

int yylex();
void yyerror(const char *s);

// The Token Vector from which parsing occurs
TokenVector *parser_tokens;

// The current index into the Token Vector
int parser_token_idx;

// The result of the parse
Prog *result;

%}

%union {
    Token *token;
    Prog *prog;
    Func *func;
    Comm *comm;
    Expr *expr;
    int name;
    FuncVector *funcs;
    ExprVector *exprs;
    IntRefVector *names;
}

%token LParen 1
%token RParen 2
%token LCurly 3
%token RCurly 4
%token Comma  5
%token While  6
%token Do     7
%token Assign 8
%token Semi   9
%token Return 10
%token Add    11
%token Sub    12
%token Name   13
%token Int    14

%type <prog> prog
%type <func> func
%type <funcs> funcs
%type <comm> comm
%type <expr> expr
%type <name> name
%type <names> names
%type <names> namescont
%type <exprs> exprs
%type <exprs> exprscont

%start prog

%%

prog:  funcs { result = Prog_init($1); }
    ;

func:   name LParen names RParen LCurly comm RCurly { $$ = Func_init($1, $3, $6); }
    ;

funcs:  func funcs { $$ = $2; FuncVector_insert($2, 0, $1); }
    |   func { $$ = FuncVector_init(); FuncVector_append($$, $1); }
    ;

comm:   While expr Do LCurly comm RCurly { $$ = While_init($2, $5); }
    |   name Assign expr { $$ = Assign_init($1, $3); }
    |   comm Semi comm { $$ = Comp_init($1, $3); }
    |   Return expr { $$ = Return_init($2); }
    ;

expr:   Int { $$ = Int_init(Token_name(yylval.token)); }
    |   expr Add expr { $$ = Add_init($1, $3); }
    |   expr Sub expr { $$ = Sub_init($1, $3); }
    |   name LParen exprs RParen { $$ = Call_init($1, $3); }
    |   name { $$ = Var_init($1); }
    ;

name:   Name { $$ = Token_name(yylval.token); }
    ;

names:  /* empty */ { $$ = IntRefVector_init(); }
    |   name namescont { IntRefVector_insert($2, 0, IntRef_init($1)); $$ = $2; }
    ;

namescont:  /* empty */ { $$ = IntRefVector_init(); }
    |   Comma name namescont { IntRefVector_insert($3, 0, IntRef_init($2)); $$ = $3; }
    ;

exprs:  /* empty */ { $$ = ExprVector_init(); }
    |   expr exprscont { ExprVector_insert($2, 0, $1); $$ = $2; }
    ;

exprscont:  /* empty */ { $$ = ExprVector_init(); }
    |   Comma expr exprscont { ExprVector_insert($3, 0, $2); $$ = $3; }
    ;
%%

/*
 * Prepare the parser. Takes an array of Tokes from which an AST [will|could] be
 * parsed. This must be called prior to parsing.
 */
Prog *parse(TokenVector *tok_vec) {
    parser_tokens = tok_vec;
    parser_token_idx = -1;
    yyparse();
    return result;
}

/*
 * This function should only be used by parser code. It moves the parser ahead
 * to the next token.
 */
int yylex() {
    parser_token_idx++;
    yylval.token = TokenVector_get(parser_tokens, parser_token_idx);
    if(Token_type(yylval.token) == endOfInput) {
        return 0;
    }
    else {
        return Token_type(yylval.token);
    }
}

/*
 * Indicate that an error occured, with a message.
 */
void yyerror(const char *s) {
    printf("Parse error on input '%s', line %d: %s\n", Token_str(yylval.token),
            Token_line(yylval.token), s);
    exit(EXIT_FAILURE);
}
