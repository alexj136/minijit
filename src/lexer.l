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
        char *text = chstrdup((ty == endOfInput) ? "EOF" : yytext); \
        int name = (ty == tokenName) ? lease_int_name(yytext) : \
                ((ty == tokenInt) ? name = atoi(yytext) : -1); \
        int line_no = yylineno; \
        char_no += yyleng; \
        return Token_init(ty, text, name, line_no, char_no - yyleng); \
    } while(0)

Token *Token_init(TokenType type, char *str, int name, int line_no,
        int char_no);
#define Token_eq(t1, t2) (t1 == t2) // Must be pointer-equal to be equal
void Token_free(Token *token);

// Stores the current position in the line, as flex can't do this
int char_no;

DEFINE_VECTORABLE(Token)

StringIntPair *StringIntPair_init(char *sval, int ival);
#define StringIntPair_eq(p1, p2) \
    (((p1->ival) == (p2->ival)) && str_equal(p1->sval, p2->sval))
void StringIntPair_free(StringIntPair *sip);

DEFINE_VECTORABLE(StringIntPair)

// Stores a mapping from string identifiers to the integer name representation
// used in ASTs. Initialised as empty by lex_file().
StringIntPairVector *name_map;

// Stores the next integer to be used as a representation of a string name.
// Initialised to 0 by lex_file().
int next_int_name;

/*
 * Retreive an integer to represent the given string name. If the string has not
 * been seen before, use the next_int_name (and increment next_int_name
 * afterwards), associating that integer with the given name in name_map. If the
 * string has been seen, just return the integer already associated with it.
 */
int lease_int_name(char *string_name) {

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

    // If we didn't find it, use the next_int_name, and add a new association to
    // the list, and increment next_int_name for next time we get here
    if(!found_name) {
        name = next_int_name;
        StringIntPairVector_append(name_map,
                StringIntPair_init(chstrdup(string_name), next_int_name));
        next_int_name++;
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
line_break [\n\r]
space [ \t\b\012]
comment #.*

%%

{line_break}    { char_no = 1;               }
{comment}       {                            }
{space}         { char_no++;                 }
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
Token *Token_init(TokenType type, char *str, int name, int line_no,
        int char_no) {

    Token *token = challoc(sizeof(Token));
    Token_type(token) = type;
    Token_str(token)  = str;
    Token_name(token) = name;
    Token_line_no(token) = line_no;
    Token_char_no(token) = char_no;
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
    printf("line=%d, column=%d.\n", Token_line_no(token), Token_char_no(token));
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
 * Free a StringIntPair. Does not free the contained string, as these will be
 * passed on to further stages of the compilation process.
 */
void StringIntPair_free(StringIntPair *sip) {
    free(sip);
    return;
}

/*
 * Create a LexerResult struct from a Token Vector and a mapping from integer
 * names to string names.
 */
LexerResult *LexerResult_init(TokenVector *tokens, charVector *name_map,
        int next_name) {

    LexerResult *lr = challoc(sizeof(LexerResult));
    lr->tokens      = tokens;
    lr->name_map    = name_map;
    lr->next_name   = next_name;
    return lr;
}

/*
 * Free a LexerResult and its constituents.
 */
void LexerResult_free(LexerResult *lr) {
    TokenVector_free_elems(lr->tokens);
    charVector_free_elems(lr->name_map);
    free(lr);
    return;
}

/*
 * Do the lexical analysis. Return the results in a LexerResult. Requires the
 * lexer to be in the ready state, for example by setting the yyin pointer to be
 * the file that is to be lexed, or by calling yy_scan_string on the string to
 * be lexed.
 */
LexerResult *do_analysis() {

    name_map      = StringIntPairVector_init();
    next_int_name = 0;
    char_no       = 1;

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

    // Create a flipped name map - Vector indices map to string names
    charVector *flipped_name_map = flip_name_map(name_map);

    // Clean up and return the results
    StringIntPairVector_free_elems(name_map);
    name_map = NULL;
    char_no = 1;
    return LexerResult_init(tok_vec, flipped_name_map, next_int_name);
}

/*
 * Perform lexical analysis on the given file. Return a pointer to a
 * LexerResult struct, containing a TokenVector and a charVector, which maps
 * integer names in the tokens to string names in the source file. Does not
 * close the file, this must be done elsewhere.
 */
LexerResult *lex_file(FILE *file) {

    // Point yyin at the given file since it opened successfully
	yyin = file;

    // Analyse the file
    LexerResult *lr = do_analysis();

    // Clean up and return the results
    yylex_destroy();
    return lr;
}

/*
 * Perform lexical analysis on the given string. Return a pointer to a
 * LexerResult struct, containing a TokenVector and a charVector, which maps
 * integer names in the tokens to string names in the source file.
 */
LexerResult *lex_string(const char *str) {

    // Set flex's state to be ready to scan the string
    YY_BUFFER_STATE buf_state = yy_scan_string(str);

    // Analyse the file
    LexerResult *lr = do_analysis();

    // Clean up and return the results
    yy_delete_buffer(buf_state);
    yylex_destroy();
    return lr;
}
