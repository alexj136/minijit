%option yylineno noyywrap

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "util.h"

#define YY_NO_INPUT

#define YY_DECL Token *yylex()

#define MAKE_TOKEN(ty) \
    do { \
        char *text = strdup(yytext); \
        int lineno = yylineno; \
        return Token_init(ty, text, lineno); \
    } while(0)

%}

int_const [0-9]+
name_const [a-zA-Z_][a-zA-Z0-9_]+
space [\n\r\ \t\b\012]

%%

{space}         ;
"("             { MAKE_TOKEN( tokenLParen ); }
")"             { MAKE_TOKEN( tokenRParen ); }
"{"             { MAKE_TOKEN( tokenLCurly ); }
"}"             { MAKE_TOKEN( tokenRCurly ); }
","             { MAKE_TOKEN( tokenComma  ); }
"while"         { MAKE_TOKEN( tokenWhile  ); }
":="            { MAKE_TOKEN( tokenAssign ); }
";"             { MAKE_TOKEN( tokenSemi   ); }
"return"        { MAKE_TOKEN( tokenReturn ); }
"+"             { MAKE_TOKEN( tokenAdd    ); }
"-"             { MAKE_TOKEN( tokenSub    ); }
{int_const}     { MAKE_TOKEN( tokenInt    ); }
{name_const}    { MAKE_TOKEN( tokenName   ); }
.               { MAKE_TOKEN( error       ); }

%%

Token *Token_init(TokenType type, char *str, int line) {
    Token *token = challoc(sizeof(Token));
    Token_type(token) = type;
    Token_str(token) = str;
    Token_line(token) = line;
    return token;
}

char *TokenType_str(TokenType ty) {
    switch(ty) {
        case tokenLParen : return "tokenLParen" ;
        case tokenRParen : return "tokenRParen" ;
        case tokenLCurly : return "tokenLCurly" ;
        case tokenRCurly : return "tokenRCurly" ;
        case tokenComma  : return "tokenComma"  ;
        case tokenWhile  : return "tokenWhile"  ;
        case tokenAssign : return "tokenAssign" ;
        case tokenSemi   : return "tokenSemi"   ;
        case tokenReturn : return "tokenReturn" ;
        case tokenAdd    : return "tokenAdd"    ;
        case tokenSub    : return "tokenSub"    ;
        case tokenName   : return "tokenName"   ;
        case tokenInt    : return "tokenInt"    ;
        case error       : return "error"       ;
    }
    puts("lexer.lex/TokenType_str(): Switch did not catch on any token type.");
    exit(EXIT_FAILURE);
}

void Token_print(Token *token) {
    printf("TOKEN: type=%s, text='%s', lineNo=%d.\n",
            TokenType_str(Token_type(token)),
            Token_str(token),
            Token_line(token));
}

void Token_free(Token *token) {
    free(Token_str(token));
    free(token);
}

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
    Token *current = yylex();
    while(current) {
        Token_print(current);
        Token_free(current);
        current = yylex();
    }
    return 0;
}
