%{

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"

/*
 * ParseError definitions
 */

ParseError *ParseError_init(char *text, int line_no, int char_no) {
    ParseError *err = challoc(sizeof(ParseError));
    err->text       = text;
    err->line_no    = line_no;
    err->char_no    = char_no;
    return err;
}

void ParseError_print(ParseError *err) {
    printf("Parse error on input '%s' at line %d, column %d.\n",
            err->text, err->line_no, err->char_no);
    return;
}

bool ParseError_eq(ParseError *err1, ParseError *err2) {
    // Considered equal when pointer-equal.
    return err1 == err2;
}

void ParseError_free(ParseError *err) {
    free(err->text);
    free(err);
    return;
}

DEFINE_VECTORABLE(ParseError)

void ParseErrorVector_print(ParseErrorVector *errs) {

    if(ParseErrorVector_size(errs) < 1) { return; }

    else {
        printf("Parse error(s) found:\n");
        int idx;
        for(idx = 0; idx < ParseErrorVector_size(errs); idx++) {
            ParseError *err = ParseErrorVector_get(errs, idx);
            printf("    '%s' at line %d, column %d.\n",
                    err->text, err->line_no, err->char_no);
        }
    }
}

/*
 * ParseResult definitions
 */

ParseResult *ParseSuccess_init(Prog *prog) {
    ParseResult *res = challoc(sizeof(ParseResult));
    res->type        = parseSuccess;
    res->prog        = prog;
    res->errors      = NULL;
    return res;
}

ParseResult *ParseFail_init(ParseErrorVector *errors) {
    ParseResult *res = challoc(sizeof(ParseResult));
    res->type        = parseFail;
    res->prog        = NULL;
    res->errors      = errors;
    return res;
}

void ParseResult_free(ParseResult *res) {
    if(res->type == parseSuccess) {
        Prog_free(res->prog);
    }
    else if(res->type == parseFail) {
        ParseErrorVector_free_elems(res->errors);
    }
    else {
        ERROR("Unrecognised ParseResult type");
    }
    free(res);
}

/*
 * Data used by the parser
 */

int yylex();
void yyerror(const char *s);

// The Token Vector from which parsing occurs
TokenVector *parser_tokens;

// The name map and next int name output by the lexer. These will be forwarded
// on in the output of the parser.
charVector *name_map;
int next_name;

// The current index into the Token Vector
int parser_token_idx;

// The current position in the file
int src_line_no;
int src_char_no;

// The result of the parse
Prog *result;

// Stores details of any errors encountered
ParseErrorVector *errors;

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
        $$ = Prog_init($1, name_map, next_name);
        result = $$;
    }
    ;

func:
    name LParen names RParen LCurly comm RCurly
    {
        $$ = Func_init_pos($1, $3, $6, @$.first_line, @$.first_column);
    }
    |
    error
    {
        // Could have any type safe thing here
        // main() { return 0 }
        $$ = Func_init(0, IntRefVector_init(), Return_init(Int_init(0)));
    }
    ;

funcs:
    func funcs
    {
        $$ = $2;
        FuncVector_insert($2, 0, $1);
    }
    |
    func
    {
        $$ = FuncVector_init();
        FuncVector_append($$, $1);
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
    |
    error
    {
        // Could have any type safe thing here
        $$ = Assign_init(0, Var_init(0));
    }
    ;

expr:
    Int
    {
        $$ = Int_init_pos(Token_name(yylval.token),
                @$.first_line, @$.first_column);
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
    |
    error
    {
        // Could have any type safe thing here
        $$ = Int_init(0);
    }
    ;

name:
    Name
    {
        $$ = Token_name(yylval.token);
    }
    |
    error
    {
        // Could have any type safe thing here
        $$ = 0;
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
        IntRefVector_insert($2, 0, IntRef_init($1));
        $$ = $2;
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
        IntRefVector_insert($3, 0, IntRef_init($2));
        $$ = $3;
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
        ExprVector_insert($2, 0, $1);
        $$ = $2;
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
        ExprVector_insert($3, 0, $2);
        $$ = $3;
    }
    ;
%%

/*
 * Do the parsing. The input is a LexerResult, and the output is a ParseResult,
 * which contains either a Prog, or a ParseErrorVector. The LexerResult will be
 * freed by this call, so do not keep it afterwards.
 */
ParseResult *parse(LexerResult *lexer_result) {

    // Initialise variables
    parser_tokens    = LexerResult_tokens(lexer_result);
    name_map         = LexerResult_name_map(lexer_result);
    next_name        = LexerResult_next_name(lexer_result);
    errors           = ParseErrorVector_init();
    result           = NULL;
    parser_token_idx = -1;

    // Do the parse
    yyparse();

    // Clean up
    TokenVector_free_elems(parser_tokens);
    free(lexer_result);
    parser_tokens = NULL;
    name_map      = NULL;
    next_name     = -1;

    if(ParseErrorVector_size(errors) > 0) {
        Prog_free(result);
        result = NULL;
        return ParseFail_init(errors);
    }
    else {
        ParseErrorVector_free_elems(errors);
        errors = NULL;
        return ParseSuccess_init(result);
    }

}

/*
 * This function should only be used by parser code. It moves the parser ahead
 * to the next token.
 */
int yylex() {

    parser_token_idx++;

    yylval.token        = TokenVector_get(parser_tokens, parser_token_idx);
    yylloc.first_line   = Token_line_no(yylval.token);
    yylloc.first_column = Token_char_no(yylval.token);

    if(Token_type(yylval.token) == endOfInput) {
        return 0;
    }
    else if(Token_type(yylval.token) == error) {
        ParseErrorVector_append(errors,
                ParseError_init(chstrdup(Token_str(yylval.token)),
                Token_line_no(yylval.token), Token_char_no(yylval.token)));
        return yylex();
    }
    else {
        return Token_type(yylval.token);
    }
}

/*
 * Indicate that an error occured, with a message.
 */
void yyerror(const char *s) {
    ParseErrorVector_append(errors,
            ParseError_init(chstrdup(Token_str(yylval.token)),
            Token_line_no(yylval.token), Token_char_no(yylval.token)));
    return;
}
