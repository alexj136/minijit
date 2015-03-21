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
        IntRefVector *prog_args = IntRefVector_init();
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);
        ICodeInterpreterState *state = ICodeInterpreterState_init(
                icodevec, 10000, TEMPORARY + 1, next_label);
        prepare_state(state, pr->prog, prog_args);
        ICodeInterpreterState_run(state);
        ASSERT(ICodeInterpreterState_result(state) == 10, "Result is correct");

        ICodeInterpreterState_free(state);
        ICodeOperationVector_free_elems(icodevec);
        IntRefVector_free_elems(prog_args);
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

            ICodeInterpreterState *state = ICodeInterpreterState_init(
                    icodevec, 10000, TEMPORARY + 1, next_label);
            IntRefVector *prog_args = IntRefVector_init();
            IntRefVector_append(prog_args, IntRef_init(arg));
            prepare_state(state, pr->prog, prog_args);
            ICodeInterpreterState_run(state);
            ASSERT(ICodeInterpreterState_result(state) == arg,
                    "Result is correct");
            IntRefVector_free_elems(prog_args);
            ICodeInterpreterState_free(state);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    END

END_TESTS
