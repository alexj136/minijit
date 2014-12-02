#ifndef lexer
#define lexer

typedef enum
    { tokenLParen
    , tokenRParen
    , tokenLCurly
    , tokenRCurly
    , tokenComma
    , tokenWhile
    , tokenAssign
    , tokenSemi
    , tokenReturn
    , tokenAdd
    , tokenSub
    , tokenName
    , tokenInt
    , endOfInput
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
void Token_print(Token *token);
void Token_free(Token *token);
void Token_arr_free(Token **tokens);
Token **lex_file(char *filename);

#endif // lexer
