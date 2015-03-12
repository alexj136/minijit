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

// The current index into the Token Vector
int parser_token_idx;

// The current position in the file
int src_line_no;
int src_char_no;

// The result of the parse
FuncVector *result;

// Stores details of any errors encountered
ParseErrorVector *errors;

%}

%locations

%union {
    Token *token;
    FuncVector *prog;
    Func *func;
    Comm *comm;
    Expr *expr;
    int name;
    FuncVector *funcs;
    ExprVector *exprs;
    IntRefVector *names;
}

/* No destructor rule for prog, as bison always discards the start symbol upon
 * returning. Obviously we don't want it to be freed as a result.
 */
%destructor { free($$);                     } func
%destructor { free($$);                     } comm
%destructor { free($$);                     } expr
%destructor { FuncVector_free($$);          } funcs
%destructor { ExprVector_free($$);          } exprs
%destructor { IntRefVector_free_elems($$);  } names

/* Token ID numbers - these must correspond to those declared in lexer.h. */
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
        $$ = $1;
        result = $$;
    }
    ;

func:
    name LParen names RParen LCurly comm RCurly
    {
        IntRefVector *local_name_map = $3; // Initially containing arguments
        Comm *body = $6;
        int num_args = IntRefVector_size(local_name_map);
        complete_local_name_map_Comm(local_name_map, body);

        $$ = Func_init_pos($1, num_args, local_name_map, body,
                @$.first_line, @$.first_column);
    }
    |
    error
    {
        // Could have any type safe thing here
        // main() { return 0 }
        $$ = Func_init(0, 0, IntRefVector_init(), Return_init(Int_init(0)));
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
    |
    error namescont
    {
        // Could have any type safe thing here
        // Just ignore the erroneous input and carry on with the list
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
    errors           = ParseErrorVector_init();
    result           = NULL;
    parser_token_idx = -1;

    // Do the parse
    yyparse();

    // Create the ParseResult & free things (different things to free depending
    // on success or failure)
    ParseResult *to_return = NULL;
    if(ParseErrorVector_size(errors) > 0) {

        to_return = ParseFail_init(errors);

        // Free lexer_result, no need to preserve name_map
        LexerResult_free(lexer_result);

        // Free the AST
        FuncVector_free_elems(result);
    }
    else {

        to_return = ParseSuccess_init(Prog_init(result,
                LexerResult_name_map(lexer_result)));

        // Free lexer_result, preserving name_map
        TokenVector_free_elems(LexerResult_tokens(lexer_result));
        free(lexer_result);

        // Free the parse errors
        ParseErrorVector_free_elems(errors);
    }

    // Clean up dangling pointers
    parser_tokens    = NULL;
    errors           = NULL;
    result           = NULL;
    parser_token_idx = -1;

    return to_return;
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
 * Called by bison when an error occurs during parsing. Adds details of the
 * error to the list of errors.
 */
void yyerror(const char *s) {
    ParseErrorVector_append(errors,
            ParseError_init(chstrdup(Token_str(yylval.token)),
            Token_line_no(yylval.token), Token_char_no(yylval.token)));
    return;
}

/*
 * Complete the local name map for a function. Takes an IntRefVector initially
 * containing the names of the function's arguments, and the body of the
 * function.
 *     The names of the variables in the function are added to the local name
 * map, and are changed within the body so that they are no longer indexes in
 * the global name map kept by the Prog, but are indexes into the local name
 * map, which yeild the index into the global name map for that variable.
 *     Representing names in this way makes the later stages of the pipeline
 * much simpler.
 */
void complete_local_name_map_Comm(IntRefVector *local_name_map, Comm *comm) {
    if(Comm_isWhile(comm)) {
        complete_local_name_map_Expr(local_name_map, While_guard(comm));
        complete_local_name_map_Comm(local_name_map, While_body(comm));
    }
    else if(Comm_isAssign(comm)) {
        Assign_name(comm) = lease_local_name(local_name_map, Assign_name(comm));
        complete_local_name_map_Expr(local_name_map, Assign_expr(comm));
    }
    else if(Comm_isComp(comm)) {
        complete_local_name_map_Comm(local_name_map, Comp_fst(comm));
        complete_local_name_map_Comm(local_name_map, Comp_snd(comm));
    }
    else if(Comm_isReturn(comm)) {
        complete_local_name_map_Expr(local_name_map, Return_expr(comm));
    }
    else {
        ERROR("Comm type not recognised.");
    }
}

/*
 * Extension of the complete_local_name_map_Comm() function for the expressions
 * contained in the commands.
 */
void complete_local_name_map_Expr(IntRefVector *local_name_map, Expr *expr) {
    if(Expr_isInt(expr)) { /* Nothing to do */ }
    else if(Expr_isAdd(expr)) {
        complete_local_name_map_Expr(local_name_map, Add_lhs(expr));
        complete_local_name_map_Expr(local_name_map, Add_rhs(expr));
    }
    else if(Expr_isSub(expr)) {
        complete_local_name_map_Expr(local_name_map, Sub_lhs(expr));
        complete_local_name_map_Expr(local_name_map, Sub_rhs(expr));
    }
    else if(Expr_isCall(expr)) {
        // Note that we do not touch the Call_name, because this is a function
        // name, not a variable name. Function names remain global after parsing
        // unlike other variable names, which become local (although we keep the
        // local name map if we ever need to retreive the original variable
        // name, e.g. for printing error messages).
        int idx;
        for(idx = 0; idx < Call_num_args(expr); idx++) {
            complete_local_name_map_Expr(local_name_map, Call_arg(expr, idx));
        }
    }
    else if(Expr_isVar(expr)) {
        Var_name(expr) = lease_local_name(local_name_map, Var_name(expr));
    }
    else {
        ERROR("Expr type not recognised.");
    }
}

/*
 * Takes a local name map and a global name. If the global name is not mapped to
 * by the given name map, first add it to the map, and then return the index of
 * its entry for insertion into an AST. If it is already in the map, just return
 * its index.
 */
int lease_local_name(IntRefVector *local_name_map, int global_name) {
    int idx = 0;
    while(idx < IntRefVector_size(local_name_map)) {
        if(IntRef_value(IntRefVector_get(local_name_map, idx)) == global_name) {
            // If we get here, we've found that the global_name was already
            // mapped to in the name map, so return the index that maps to it.
            return idx;
        }
        else {
            idx++;
        }
    }

    // If we get here, the global_name was not in the map, so add it to the map
    // before returning its index.
    IntRefVector_append(local_name_map, IntRef_init(global_name));
    return idx;
}
