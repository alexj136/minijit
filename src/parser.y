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
charVector *name_map;
int next_name;

// The current index into the Token Vector
int parser_token_idx;

// The current position in the file
int src_line_no;
int src_char_no;

// The result of the parse
Prog *result;

%}

%locations

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

prog:
    funcs
    {
        result = Prog_init($1, name_map, next_name);
    }
    ;

func:
    name LParen names RParen LCurly comm RCurly
    {
        $$ = Func_init_pos($1, $3, $6, @$.first_line, @$.first_column);
    }
    ;

funcs:
    func funcs
    {
        $$ = $2; FuncVector_insert($2, 0, $1);
    }
    |
    func
    {
        $$ = FuncVector_init(); FuncVector_append($$, $1);
    }
    ;

comm:
    While expr Do LCurly comm RCurly
    {
        $$ = While_init_pos($2, $5, @$.first_line, @$.first_column);
    }
    |
    name Assign expr
    {
        $$ = Assign_init_pos($1, $3, @$.first_line, @$.first_column);
    }
    |
    comm Semi comm
    {
        $$ = Comp_init_pos($1, $3, @$.first_line, @$.first_column);
    }
    |
    Return expr
    {
        $$ = Return_init_pos($2, @$.first_line, @$.first_column);
    }
    ;

expr:
    Int
    {
        $$ = Int_init_pos(Token_name(yylval.token), @$.first_line, @$.first_column);
    }
    |
    expr Add expr
    {
        $$ = Add_init_pos($1, $3, @$.first_line, @$.first_column);
    }
    |
    expr Sub expr
    {
        $$ = Sub_init_pos($1, $3, @$.first_line, @$.first_column);
    }
    |
    name LParen exprs RParen
    {
        $$ = Call_init_pos($1, $3, @$.first_line, @$.first_column);
    }
    |
    name
    {
        $$ = Var_init_pos($1, @$.first_line, @$.first_column);
    }
    ;

name:
    Name
    {
        $$ = Token_name(yylval.token);
    }
    ;

names:
    /* empty */
    {
        $$ = IntRefVector_init();
    }
    |
    name namescont
    {
        IntRefVector_insert($2, 0, IntRef_init($1)); $$ = $2;
    }
    ;

namescont:
    /* empty */
    {
        $$ = IntRefVector_init();
    }
    |
    Comma name namescont
    {
        IntRefVector_insert($3, 0, IntRef_init($2)); $$ = $3;
    }
    ;

exprs:
    /* empty */
    {
        $$ = ExprVector_init();
    }
    |
    expr exprscont
    {
        ExprVector_insert($2, 0, $1); $$ = $2;
    }
    ;

exprscont:
    /* empty */
    {
        $$ = ExprVector_init();
    }
    |
    Comma expr exprscont
    {
        ExprVector_insert($3, 0, $2); $$ = $3;
    }
    ;
%%

/*
 * Do the parsing. The input is a LexerResult, and the output is a Prog. The
 * LexerResult will be freed by this call, so do not keep it afterwards.
 */
Prog *parse(LexerResult *lexer_result) {

    parser_tokens = LexerResult_tokens(lexer_result);
    name_map = LexerResult_name_map(lexer_result);
    next_name = LexerResult_next_name(lexer_result);

    parser_token_idx = -1;
    yyparse();

    // Clean up
    TokenVector_free_elems(parser_tokens);
    free(lexer_result);
    parser_tokens = NULL;
    name_map = NULL;
    next_name = -1;

    return result;
}

/*
 * This function should only be used by parser code. It moves the parser ahead
 * to the next token.
 */
int yylex() {
    parser_token_idx++;
    yylval.token = TokenVector_get(parser_tokens, parser_token_idx);
    yylloc.first_line = Token_line_no(yylval.token);
    yylloc.first_column = Token_char_no(yylval.token);
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
    printf("Parse error on input '%s' at line %d, column %d: %s\n",
            Token_str(yylval.token), Token_line_no(yylval.token),
            Token_char_no(yylval.token), s);
    exit(EXIT_FAILURE);
}
