%option yylineno noyywrap

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "util.h"

#define YY_NO_INPUT

#define YY_DECL Token *next_token()

#define MAKE_TOKEN(ty) \
    do { \
        char *text = strdup(yytext); \
        int lineno = yylineno; \
        return Token_init(ty, text, lineno); \
    } while(0)

DEFINE_VECTORABLE(Token)
Token *Token_init(TokenType type, char *str, int line);
void Token_free(Token *token);

%}

int_const [0-9]+
name_const [a-zA-Z_][a-zA-Z0-9_]*
space [\n\r\ \t\b\012]

%%

{space}         ;
"("             { MAKE_TOKEN( tokenLParen ); }
")"             { MAKE_TOKEN( tokenRParen ); }
"{"             { MAKE_TOKEN( tokenLCurly ); }
"}"             { MAKE_TOKEN( tokenRCurly ); }
","             { MAKE_TOKEN( tokenComma  ); }
"while"         { MAKE_TOKEN( tokenWhile  ); }
"do"            { MAKE_TOKEN( tokenDo     ); }
":="            { MAKE_TOKEN( tokenAssign ); }
";"             { MAKE_TOKEN( tokenSemi   ); }
"return"        { MAKE_TOKEN( tokenReturn ); }
"+"             { MAKE_TOKEN( tokenAdd    ); }
"-"             { MAKE_TOKEN( tokenSub    ); }
{int_const}     { MAKE_TOKEN( tokenInt    ); }
{name_const}    { MAKE_TOKEN( tokenName   ); }
<<EOF>>         { MAKE_TOKEN( endOfInput  ); }
.               { MAKE_TOKEN( error       ); }

%%

/*
 * Initialise a new Token from the given TokenType, text and line number.
 */
Token *Token_init(TokenType type, char *str, int line) {
    Token *token = challoc(sizeof(Token));
    Token_type(token) = type;
    Token_str(token) = str;
    Token_line(token) = line;
    return token;
}

/*
 * Obtain a string description of the given TokenType.
 */
char *TokenType_str(TokenType ty) {
    switch(ty) {
        case tokenLParen : return "tokenLParen" ;
        case tokenRParen : return "tokenRParen" ;
        case tokenLCurly : return "tokenLCurly" ;
        case tokenRCurly : return "tokenRCurly" ;
        case tokenComma  : return "tokenComma"  ;
        case tokenWhile  : return "tokenWhile"  ;
        case tokenDo     : return "tokenDo"     ;
        case tokenAssign : return "tokenAssign" ;
        case tokenSemi   : return "tokenSemi"   ;
        case tokenReturn : return "tokenReturn" ;
        case tokenAdd    : return "tokenAdd"    ;
        case tokenSub    : return "tokenSub"    ;
        case tokenName   : return "tokenName"   ;
        case tokenInt    : return "tokenInt"    ;
        case endOfInput  : return "endOfInput"  ;
        case error       : return "error"       ;
    }
    puts("lexer.lex/TokenType_str(): Switch did not catch on any token type.");
    exit(EXIT_FAILURE);
}

/*
 * Print a description of the given Token to stdout.
 */
void Token_print(Token *token) {
    printf("TOKEN: type=%s, text='%s', lineNo=%d.\n",
            TokenType_str(Token_type(token)),
            Token_str(token),
            Token_line(token));
    return;
}

/*
 * Deallocate the given Token and its contained text.
 */
void Token_free(Token *token) {
    free(Token_str(token));
    free(token);
    return;
}

/*
 * Perform lexical analysis on the given file. Return a pointer to a Vector of
 * Tokens.
 */
TokenVector *lex_file(char *filename) {

    // Try to open the file
	FILE *file = fopen(filename, "r");

    // If it can't be opened, quit with an appropriate error message
	if (!file) {
        printf("lexer.lex/lef_file(): Could not open file: '%s'.\n", filename);
        exit(EXIT_FAILURE);
	}

    // Point yyin at the given file since it opened successfully
	yyin = file;

    // Initialise the Token Vector
    TokenVector *tok_vec = TokenVector_init();

    // Analyse the file
    for( ; ; ) {

        // Put the next Token into the Vector
        TokenVector_append(tok_vec, next_token());

        // Stop if the last token was an endOfInput
        if(Token_type(TokenVector_get(tok_vec,
                TokenVector_size(tok_vec) - 1)) == endOfInput) { break; }
    }

    // Close the file and return the array
    fclose(file);
    return tok_vec;
}

/*
 * Check if a Token array contains errors. Print error details if found. Return
 * true if there are no errors, false if there are.
 */
bool verify_lex(TokenVector *tok_vec) {
    int idx = 0;
    bool no_errors = true;
    for(idx = 0; idx < TokenVector_size(tok_vec); idx++) {
        Token *current = TokenVector_get(tok_vec, idx);
        if(Token_type(current) == error) {
            no_errors = false;
            printf("Lexical error on line %d: '%s'.\n", Token_line(current),
                    Token_str(current));
        }
    }
    return no_errors;
}
