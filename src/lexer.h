#ifndef util
#include "util.h"
#endif // util

#ifndef lexer
#define lexer

/*
 * Token datatype definitions
 */

typedef enum
    { tokenLParen = 1
    , tokenRParen = 2
    , tokenLCurly = 3
    , tokenRCurly = 4
    , tokenComma  = 5
    , tokenWhile  = 6
    , tokenDo     = 7
    , tokenAssign = 8
    , tokenSemi   = 9
    , tokenReturn = 10
    , tokenAdd    = 11
    , tokenSub    = 12
    , tokenName   = 13
    , tokenInt    = 14
    , endOfInput  = 15
    , error       = 16
    } TokenType;

#define Token_type(token) token->type
#define Token_str(token) token->str
#define Token_name(token) token->name
#define Token_line_no(token) token->line_no
#define Token_char_no(token) token->char_no

typedef struct Token Token;
struct Token {
    TokenType type;
    char *str;
    int name;
    int line_no;
    int char_no;
};

FORWARD_DECLARE_VECTORABLE(Token)

void Token_print(Token *token);
void Token_free(Token *token);
bool verify_lex(TokenVector *tok_vec);
void print_errors(TokenVector *tok_vec);

/*
 * StringIntPair datatype definitions. Used to store mappings between string
 * identifier names and the integer name representation used in ASTs.
 */

#define StringIntPair_str(sip) sip->sval
#define StringIntPair_int(sip) sip->ival

typedef struct StringIntPair StringIntPair;
struct StringIntPair {
    char *sval;
    int ival;
};

FORWARD_DECLARE_VECTORABLE(StringIntPair)

/*
 * The output of the lexer is a list of tokens and a map from integer names to
 * string names. These are returned in a pair in a LexerResult struct.
 */

#define LexerResult_tokens(lr) lr->tokens
#define LexerResult_name_map(lr) lr->name_map

typedef struct LexerResult LexerResult;
struct LexerResult {
    TokenVector *tokens;
    charVector *name_map;
};

LexerResult *LexerResult_init(TokenVector *tokens, charVector *name_map);
void LexerResult_free(LexerResult *lr);

// The lexer functions
LexerResult *lex_file(FILE *file);
LexerResult *lex_string(const char *str);

#endif // lexer
