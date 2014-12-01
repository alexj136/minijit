#ifndef lexer
#define lexer

typedef enum
    { tokenOpenBr
    , tokenClosBr
    , tokenOpenCr
    , tokenClosCr
    , tokenComma
    , tokenWhile
    , tokenAssign
    , tokenSemi
    , tokenReturn
    , tokenAdd
    , tokenSub
    , tokenName
    , tokenInt
    , error
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

Token *Token_init(TokenType type, char *str, int line);
Token *get_next_Token();
void Token_print(Token *token);
void Token_free(Token *token);

#endif // lexer
