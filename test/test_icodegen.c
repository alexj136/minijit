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

    TEST("Compile an addition program, check that it gives the correct "
            "result when run with various arguments")

        char *prog = "main(a, b) { return a + b }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int idx;
        for(idx = 0; idx < 100; idx++) {

            int a = (rand() % 10000) - 5000;
            int b = (rand() % 10000) - 5000;

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(a));
            IntRefVector_append(initial_stack, IntRef_init(b));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack)
                    == a + b, "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

    TEST("Compile a subtraction program, check that it gives the correct "
            "result when run with various arguments")

        char *prog = "main(a, b) { return a - b }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int idx;
        for(idx = 0; idx < 100; idx++) {

            int a = (rand() % 10000) - 5000;
            int b = (rand() % 10000) - 5000;

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(b));
            IntRefVector_append(initial_stack, IntRef_init(a));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack)
                    == a - b, "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

    TEST("Compile a program with assignment, check that it gives the correct "
            "result when run with various arguments")

        char *prog = "main(a) { a := a + 1; return a }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int idx;
        for(idx = 0; idx < 100; idx++) {

            int a = (rand() % 10000) - 5000;

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(a));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack)
                    == a + 1, "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

    TEST("Compile a program with a while loop, check that it gives the correct "
            "result when run with various arguments")

        char *prog = "  main(a, b) {            " // product of a and b where a
                     "      c := a;             " // and b are greater than or
                     "      while b - 1 do {    " // equal to 1
                     "          b := b - 1;     "
                     "          a := a + c      "
                     "      };                  "
                     "      return a            "
                     "  }                       ";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int idx;
        for(idx = 0; idx < 100; idx++) {

            int a = (rand() % 100) + 1;
            int b = (rand() % 100) + 1;

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(0));
            IntRefVector_append(initial_stack, IntRef_init(b));
            IntRefVector_append(initial_stack, IntRef_init(a));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack)
                    == a * b, "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

    TEST("Compile a program with a function call, check that it gives the "
            "correct result")

        char *prog = "  main(a) {               "
                     "      return foo(a) + a   "
                     "  }                       "
                     "  foo(a) {                "
                     "      return a + 10       "
                     "  }                       ";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int idx;
        for(idx = 0; idx < 100; idx++) {

            int a = (rand() % 10000) - 5000;

            IntRefVector *initial_stack = IntRefVector_init();
            IntRefVector_append(initial_stack, IntRef_init(a));

            ASSERT(ICodeOperationVector_execute(icodevec, initial_stack)
                    == (2 * a) + 10, "Result is correct");

            IntRefVector_free_elems(initial_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END
END_TESTS
