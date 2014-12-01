%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "util.h"

Token *Token_init(TokenType type, char *str, int line) {
    Token *token = challoc(sizeof(Token));
    Token_type(token) = type;
    Token_str(token) = str;
    Token_line(token) = line;
    return token;
}

Token *get_next_Token() {
    TokenType ty = yylex();
    char *text = strdup(yytext);
    int lineno = yylineno;
    return Token_init(ty, text, lineno);
}

char *TokenType_str(TokenType ty) {
    switch(ty) {
        case tokenOpenBr: return "tokenOpenBr";
        case tokenClosBr: return "tokenClosBr";
        case tokenOpenCr: return "tokenOpenCr";
        case tokenClosCr: return "tokenClosCr";
        case tokenComma : return "tokenComma";
        case tokenWhile : return "tokenWhile";
        case tokenAssign: return "tokenAssign";
        case tokenSemi  : return "tokenSemi";
        case tokenReturn: return "tokenReturn";
        case tokenAdd   : return "tokenAdd";
        case tokenSub   : return "tokenSub";
        case tokenName  : return "tokenName";
        case tokenInt   : return "tokenInt";
        case error      : return "error";
    }
    puts("lexer.lex/TokenType_str(): Switch did not catch on any token type.");
    exit(EXIT_FAILURE);
}

void Token_print(Token *token) {
    printf("TOKEN: type=%s, text=%s, lineNo=%d.\n",
            TokenType_str(Token_type(token)),
            Token_str(token),
            Token_line(token));
}

void Token_free(Token *token) {
    free(Token_str(token));
    free(token);
}

%}

int_const [0-9]+
name_const [a-zA-Z_][a-zA-Z0-9_]+
space [\n\r\ \t\b\012]

%%

"("             { return tokenOpenBr ; }
")"             { return tokenClosBr ; }
"{"             { return tokenOpenCr ; }
"}"             { return tokenClosCr ; }
","             { return tokenComma  ; }
"while"         { return tokenWhile  ; }
":="            { return tokenAssign ; }
";"             { return tokenSemi   ; }
"return"        { return tokenReturn ; }
"+"             { return tokenAdd    ; }
"-"             { return tokenSub    ; }
{int_const}     { return tokenInt    ; }
{name_const}    { return tokenName   ; }
.               { return error       ; }
%%

int yywrap() { return 1; }

int main(int argc, char *argv[]) {
    if(argc < 2) {
        puts("No arguments provided.");
        exit(EXIT_FAILURE);
    }
	FILE *file = fopen(argv[1], "r");
	if (!file) {
        printf("Could not open file: '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
	}
	yyin = file;
    Token *current = get_next_Token();
    while(current) {
        Token_print(current);
        current = get_next_Token();
    }
    return 0;
}
