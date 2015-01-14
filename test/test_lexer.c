#include "minunit.h"
#include "lexer.h"

#define ASSERT_TOKEN_TYPE(token, type) \
    ASSERT(Token_type(token) == type, "unexpected token type")

MINUNIT_TESTS

    TEST("Single token lexer test")
        char *text = "while";
        LexerResult *lr = lex_string(text);
        TokenVector *tks = LexerResult_tokens(lr);

        // A while token and an end of input token.
        ASSERT(TokenVector_size(tks) == 2, "TokenVector has incorrect length");
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 0), tokenWhile);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 1), endOfInput);
        
        LexerResult_free(lr);
    END

    TEST("Simple lexer test")
        char *text = " while  + +  - ; hello";
        LexerResult *lr = lex_string(text);
        TokenVector *tks = LexerResult_tokens(lr);

        ASSERT(TokenVector_size(tks) == 7, "TokenVector has incorrect length");
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 0), tokenWhile);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 1), tokenAdd);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 2), tokenAdd);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 3), tokenSub);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 4), tokenSemi);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 5), tokenName);
        ASSERT_TOKEN_TYPE(TokenVector_get(tks, 6), endOfInput);
        
        LexerResult_free(lr);
    END

    TEST("Incorrect token test")
        char *text = "£ $ % * ] @ ?";
        LexerResult *lr = lex_string(text);
        TokenVector *tks = LexerResult_tokens(lr);

        // Make sure all tokens are error except the endOfInput
        int idx;
        for(idx = 0; idx < TokenVector_size(tks) - 1; idx++) {
            ASSERT_TOKEN_TYPE(TokenVector_get(tks, idx), error);
        }
        ASSERT_TOKEN_TYPE(
                TokenVector_get(tks, TokenVector_size(tks) - 1), endOfInput);
        
        LexerResult_free(lr);
    END

END_TESTS
