#include "minunit.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "icode.h"
#include "icodegen.h"
#include "icode_interpreter.h"

MINUNIT_TESTS

    TEST("Compile a simple program, check that it gives the correct result "
            "when run")

        char *prog = "main() { return 10 }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        IntRefVector *initial_stack = IntRefVector_init();
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        ASSERT(ICodeOperationVector_execute(icodevec, initial_stack) == 10,
                "Result is correct");

        ICodeOperationVector_free_elems(icodevec);
        IntRefVector_free_elems(initial_stack);
        ParseResult_free(pr);
    END

    TEST("Compile an identity program, check that it gives the correct "
            "result when run with various arguments")

        char *prog = "main(a) { return a }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int arg;
        for(arg = 0; arg < 100; arg++) {

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(arg));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack) == arg,
                    "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

END_TESTS
