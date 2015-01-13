#include "minunit.h"
#include "lexer.h"

MINUNIT_TESTS

    TEST("Single token lexer test")
        char *text = "while";
        LexerResult *lr = lex_string(text);
        TokenVector *tks = LexerResult_tokens(lr);

        // A while token and an end of input token.
        ASSERT(TokenVector_size(tks) == 2, "TokenVector has incorrect length");
        ASSERT(Token_type(TokenVector_get(tks, 0)) == tokenWhile,
                "incorrect token type");
        ASSERT(Token_type(TokenVector_get(tks, 1)) == endOfInput,
                "incorrect token type");
        
        LexerResult_free(lr);
    END

    TEST("Another single token lexer test")
        char *text = ";";
        LexerResult *lr = lex_string(text);
        TokenVector *tks = LexerResult_tokens(lr);

        // A semicolon token and an end of input token.
        ASSERT(TokenVector_size(tks) == 2, "TokenVector has incorrect length");
        ASSERT(Token_type(TokenVector_get(tks, 0)) == tokenSemi,
                "incorrect token type");
        ASSERT(Token_type(TokenVector_get(tks, 1)) == endOfInput,
                "incorrect token type");
        
        LexerResult_free(lr);
    END

END_TESTS
