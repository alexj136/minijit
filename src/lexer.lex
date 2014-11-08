%option yylineno

%{

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

"("             { return Token_init(tokenOpenBr, yytext, yylineno); }
")"             { return Token_init(tokenClosBr, yytext, yylineno); }
"{"             { return Token_init(tokenOpenCr, yytext, yylineno); }
"}"             { return Token_init(tokenClosCr, yytext, yylineno); }
","             { return Token_init(tokenComma , yytext, yylineno); }
"while"         { return Token_init(tokenWhile , yytext, yylineno); }
":="            { return Token_init(tokenAssign, yytext, yylineno); }
";"             { return Token_init(tokenSemi  , yytext, yylineno); }
"return"        { return Token_init(tokenReturn, yytext, yylineno); }
"+"             { return Token_init(tokenAdd   , yytext, yylineno); }
"-"             { return Token_init(tokenSub   , yytext, yylineno); }
{int_const}     { return Token_init(tokenInt   , yytext, yylineno); }
{name_const}    { return Token_init(tokenName  , yytext, yylineno); }
.               { return Token_init(error      , yytext, yylineno); }
