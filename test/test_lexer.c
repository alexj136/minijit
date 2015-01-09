#include "minunit.h"
#include "lexer.h"

MINUNIT_TESTS

    TEST("")
        char *text = "while a - 1 do {return a}";
        LexerResult *lr = lex_file(text);
    END

END_TESTS
