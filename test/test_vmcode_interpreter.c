#include "minunit.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "icode.h"
#include "icodegen.h"

MINUNIT_TESTS
    TEST("Simple test of the VMCode interpreter")

        char *prog = "main() { return 10 }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);
        ICodeOperationVector *icodevec =
                icodegen_Prog(pr->prog, ({ int next_label = 0; &next_label; }));
        IntRefVector *initial_stack = IntRefVector_init();

        ASSERT(VMCode_execute(icodevec, initial_stack) == 10,
                "Result is correct");

        ICodeOperationVector_free_elems(icodevec);
        IntRefVector_free_elems(initial_stack);
        ParseResult_free(pr);

    END
END_TESTS
