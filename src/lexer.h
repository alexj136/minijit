#ifndef util
#include "util.h"
#endif // util

#ifndef lexer
#define lexer

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
#define Token_line(token) token->line

typedef struct Token Token;
struct Token {
    TokenType type;
    char *str;
    int line;
};

FORWARD_DECLARE_VECTORABLE(Token)

void Token_print(Token *token);
void Token_free(Token *token);
TokenVector *lex_file(char *filename);
bool verify_lex(TokenVector *tok_vec);

#endif // lexer
