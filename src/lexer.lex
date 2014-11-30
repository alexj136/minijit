%option yylineno

%{

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

void Token_free(Token *token) {
    free(Token_str(token));
    free(token);
}

%}

int_const [0-9]+
name_const [a-zA-Z_][a-zA-Z0-9_]+
space [\n\r\ \t\b\012]

%%

"("             { return Token_init(tokenOpenBr, strdup(yytext), yylineno); }
")"             { return Token_init(tokenClosBr, strdup(yytext), yylineno); }
"{"             { return Token_init(tokenOpenCr, strdup(yytext), yylineno); }
"}"             { return Token_init(tokenClosCr, strdup(yytext), yylineno); }
","             { return Token_init(tokenComma , strdup(yytext), yylineno); }
"while"         { return Token_init(tokenWhile , strdup(yytext), yylineno); }
":="            { return Token_init(tokenAssign, strdup(yytext), yylineno); }
";"             { return Token_init(tokenSemi  , strdup(yytext), yylineno); }
"return"        { return Token_init(tokenReturn, strdup(yytext), yylineno); }
"+"             { return Token_init(tokenAdd   , strdup(yytext), yylineno); }
"-"             { return Token_init(tokenSub   , strdup(yytext), yylineno); }
{int_const}     { return Token_init(tokenInt   , strdup(yytext), yylineno); }
{name_const}    { return Token_init(tokenName  , strdup(yytext), yylineno); }
.               { return Token_init(error      , strdup(yytext), yylineno); }
%%

int yywrap() { return 1; }

int main(int argc, char *argv[]) {
    if(argc < 2) { puts("No arguments provided."); }
	FILE *file = fopen(argv[1], "r");
	if (!file) {
        printf("Could not open file: '%s'.", argv[1]);
		return -1;
	}
	yyin = file;
	yylex();
    return 0;
}
