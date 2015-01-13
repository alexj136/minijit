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
        int name = (ty == tokenName) ? leaseIntName(yytext) : \
                ((ty == tokenInt) ? name = atoi(yytext) : -1); \
        int lineno = yylineno; \
        return Token_init(ty, text, name, lineno); \
    } while(0)

Token *Token_init(TokenType type, char *str, int name, int line);
void Token_free(Token *token);

DEFINE_VECTORABLE(Token)

StringIntPair *StringIntPair_init(char *sval, int ival);
void StringIntPair_free(StringIntPair *sip);

DEFINE_VECTORABLE(StringIntPair)

// Stores a mapping from string identifiers to the integer name representation
// used in ASTs. Initialised as empty by lex_file().
StringIntPairVector *name_map;

// Stores the next integer to be used as a representation of a string name.
// Initialised to 0 by lex_file().
int nextIntName;

/*
 * Retreive an integer to represent the given string name. If the string has not
 * been seen before, use the nextIntName (and increment nextIntName afterwards),
 * associating that integer with the given name in name_map. If the string has
 * been seen, just return the integer already associated with it.
 */
int leaseIntName(char *string_name) {

    int name;

    // Look for the string name in the list
    bool found_name = false;
    int idx = 0;
    while((!found_name) && (idx < StringIntPairVector_size(name_map))) {
        StringIntPair *current = StringIntPairVector_get(name_map, idx);

        // If we find it, record the corresponding name and quit the loop
        if(str_equal(string_name, StringIntPair_str(current))) {
            name = StringIntPair_int(current);
            found_name = true;
        }
        idx++;
    }

    // If we didn't find it, use the nextIntName, and add a new association to
    // the list, and increment nextIntName for next time we get here
    if(!found_name) {
        name = nextIntName;
        StringIntPairVector_append(name_map,
                StringIntPair_init(strdup(string_name), nextIntName));
        nextIntName++;
    }

    return name;
}

/*
 * Convert a mapping from integer names to string names, into a mapping from
 * string names to integer names. Used in lex_file. Returns a vector of strings
 * that index according to their originally mapped integers.
 */
charVector *flip_name_map(StringIntPairVector *name_map) {

    // Vector to return return as result
    charVector *flipped_name_map = charVector_init();

    int idx;
    for(idx = 0; idx < StringIntPairVector_size(name_map); idx++) {

        int jdx = 0;
        while(jdx < StringIntPairVector_size(name_map)) {
            if(StringIntPair_int(StringIntPairVector_get(
                    name_map, jdx)) == idx) {

                break;
            }
            jdx++;
        }
        charVector_append(flipped_name_map, StringIntPair_str(
                StringIntPairVector_get(name_map, jdx)));
    }

    return flipped_name_map;
}

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
 * Initialise a new Token from the given TokenType, name and line number. Use
 * -1 as the name for non-identifier tokens.
 */
Token *Token_init(TokenType type, char *str, int name, int line) {
    Token *token = challoc(sizeof(Token));
    Token_type(token) = type;
    Token_str(token)  = str;
    Token_name(token) = name;
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
    ERROR("Switch did not catch on any token type.");
}

/*
 * Print a description of the given Token to stdout.
 */
void Token_print(Token *token) {
    printf("TOKEN: type=%s, ", TokenType_str(Token_type(token)));
    printf("text='%s', ", Token_str(token));
    if(Token_type(token) == tokenName) {
        printf("integer name=%d, ", Token_name(token));
    }
    else if(Token_type(token) == tokenInt) {
        printf("integer value=%d, ", Token_name(token));
    }
    printf("lineNo=%d.\n", Token_line(token));
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
 * Create a new StringIntPair and return a pointer to it.
 */
StringIntPair *StringIntPair_init(char *sval, int ival) {
    StringIntPair *sip = challoc(sizeof(StringIntPair));
    StringIntPair_str(sip) = sval;
    StringIntPair_int(sip) = ival;
    return sip;
}

/*
 * Free a StringIntPair. Frees the contained string.
 */
void StringIntPair_free(StringIntPair *sip) {
    free(StringIntPair_str(sip));
    free(sip);
    return;
}

/*
 * Create a LexerResult struct from a Token Vector and a mapping from integer
 * names to string names.
 */
LexerResult *LexerResult_init(TokenVector *tokens, charVector *name_map) {
    LexerResult *lr = challoc(sizeof(LexerResult));
    LexerResult_tokens(lr) = tokens;
    LexerResult_name_map(lr) = name_map;
    return lr;
}

/*
 * Free a LexerResult and its constituents.
 */
void LexerResult_free(LexerResult *lr) {
    TokenVector_free_elems(LexerResult_tokens(lr));
    charVector_free_elems(LexerResult_name_map(lr));
    free(lr);
    return;
}

/*
 * Do the lexical analysis. Return the results in a TokenVector. Requires the
 * lexer to be in the ready state, for example by setting the yyin pointer to be
 * the file that is to be lexed.
 */
TokenVector *do_analysis() {

    // Initialise the Token Vector for the results
    TokenVector *tok_vec = TokenVector_init();

    // Do the analysis
    for( ; ; ) {

        // Put the next Token into the Vector
        TokenVector_append(tok_vec, next_token());

        // Stop if the last token was an endOfInput
        if(Token_type(TokenVector_get(tok_vec,
                TokenVector_size(tok_vec) - 1)) == endOfInput) { break; }
    }

    // Return the results
    return tok_vec;
}

/*
 * Perform lexical analysis on the given file. Return a pointer to a Vector of
 * Tokens.
 */
LexerResult *lex_file(FILE *file) {

    name_map = StringIntPairVector_init();
    nextIntName = 0;

    // Point yyin at the given file since it opened successfully
	yyin = file;

    // Analyse the file
    TokenVector *tokens = do_analysis();

    // Create a flipped name map - Vector indices map to string names
    charVector *flipped_name_map = flip_name_map(name_map);

    // Clean up and return the results
    StringIntPairVector_free(name_map);
    name_map = NULL;
    nextIntName = 0;
    return LexerResult_init(tokens, flipped_name_map);
}

/*
 * Perform lexical analysis on the given string. Return a pointer to a Vector of
 * tokens.
 */
LexerResult *lex_string(const char *str) {

    name_map = StringIntPairVector_init();
    nextIntName = 0;

    // Set flex's state to be ready to scan the string
    YY_BUFFER_STATE buf_state = yy_scan_string(str);

    // Analyse the file
    TokenVector *tokens = do_analysis();

    // Create a flipped name map - Vector indices map to string names
    charVector *flipped_name_map = flip_name_map(name_map);

    // Clean up and return the results
    yy_delete_buffer(buf_state);
    StringIntPairVector_free(name_map);
    name_map = NULL;
    nextIntName = 0;
    return LexerResult_init(tokens, flipped_name_map);
}

/*
 * Check if a Token array contains errors. Print error details if found. Return
 * true if there are no errors, false if there are.
 */
bool verify_lex(TokenVector *tok_vec) {
    bool no_errors = true; // Is the token list error-free?

    // For each token...
    int idx = 0;
    while(no_errors && idx < TokenVector_size(tok_vec)) {
        Token *current = TokenVector_get(tok_vec, idx);

        // If the token is an error, set no_errors to false
        if(Token_type(current) == error) {
            no_errors = false;
        }

        idx++;
    }

    // True if no errors were found, false if errors were found
    return no_errors;
}

/*
 * Print information on any errors that may exist in a TokenVector.
 */
void print_errors(TokenVector *tok_vec) {
    int idx;
    for(idx = 0; idx < TokenVector_size(tok_vec); idx++) {
        Token *current = TokenVector_get(tok_vec, idx);

        // If the token is an error, print details and set no_errors to false
        if(Token_type(current) == error) {
            printf("Invalid text on line %d: '%s'.\n", Token_line(current),
                    Token_str(current));
        }
    }
    return;
}
