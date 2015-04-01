#include "minunit.h"
#include "testing_programs.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "icode.h"
#include "icodegen.h"
#include "vmcode_interpreter.h"

MINUNIT_TESTS

    TEST("Simple test of the VMCode interpreter")

        char *prog = "main() { return 10 }";
        LexerResult *lr = lex_string(prog);
        ParseResult *pr = parse(lr);
        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);
        IntRefVector *initial_stack = IntRefVector_init();

        ASSERT(VMCode_execute(icodevec, initial_stack) == 10,
                "Result is correct");

        ICodeOperationVector_free_elems(icodevec);
        IntRefVector_free_elems(initial_stack);
        ParseResult_free(pr);

    END

    TEST("Test the VMCode interpreter with the test program module programs")

    prepare_testing_programs();

    int test_prog_idx;
    for(test_prog_idx = 0; test_prog_idx < num_testing_programs();
            test_prog_idx++) {

        TestProgram *tp = get_test_program(test_prog_idx);
        LexerResult *lr = lex_string(tp->prog_str);
        ParseResult *pr = parse(lr);

        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(pr->prog, &next_label);

        int num_runs;
        for(num_runs = 0; num_runs < 1000; num_runs++) {

            IntRefVector *args_to_c_version = IntRefVector_init();
            IntRefVector *initial_vm_stack = IntRefVector_init();

            int arg_num;
            for(arg_num = 0; arg_num < tp->arity; arg_num++) {
                int arg = (rand() % 10000) - 5000;
                IntRefVector_append(args_to_c_version, IntRefVector_init(arg));
                IntRefVector_insert(initial_vm_stack, 0,
                        IntRefVector_init(arg));
            }

            // Run the programs

            IntRefVector_free_elems(args_to_c_version);
            IntRefVector_free_elems(initial_vm_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    }

    destroy_testing_programs();

    END
END_TESTS
