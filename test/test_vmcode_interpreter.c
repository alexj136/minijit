#include "minunit.h"
#include "testing_programs.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "icode.h"
#include "icodegen.h"
#include "vmcode_interpreter.h"

MINUNIT_TESTS

    TEST("Test the VMCode interpreter with the test program module programs")

    prepare_testing_programs();

    int test_prog_idx;
    for(test_prog_idx = 0; test_prog_idx < num_testing_programs();
            test_prog_idx++) {

        // Get the next test program struct, parse the minijit program
        TestProgram *tp = get_test_program(test_prog_idx);
        LexerResult *lr = lex_string(tp->prog_str);
        ParseResult *pr = parse(lr);
        Prog *prog      = pr->prog;

        // Create a function pointer to the C version of the test program
        int (*c_version)(IntRefVector *) = tp->c_version;

        // Generate icode (VMCode) from the parsed minijit program
        int next_label = 0;
        ICodeOperationVector *icodevec = icodegen_Prog(prog, &next_label);

        // Run the C version and VMCode versions many times and assert that the
        // results are always the same
        int num_runs;
        for(num_runs = 0; num_runs < 1000; num_runs++) {

            // Consruct the arguments/initial stack for this program
            IntRefVector *args_to_c_version = IntRefVector_init();
            IntRefVector *initial_vm_stack = IntRefVector_init();

            int arg_num;
            for(arg_num = 0; arg_num < tp->arity; arg_num++) {
                int arg = (rand() % 10000) - 5000;
                IntRefVector_append(args_to_c_version, IntRef_init(arg));
                IntRefVector_insert(initial_vm_stack, 0, IntRef_init(arg));
            }
            int var_num;
            for(var_num = arg_num;
                    var_num < Func_num_vars(Prog_func(prog, 0)); var_num++) {

                IntRefVector_insert(initial_vm_stack, 0, IntRef_init(0));
            }

            // Run the programs
            int vmcode_result = VMCode_execute(icodevec, initial_vm_stack);
            int c_result      = c_version(args_to_c_version);

            ASSERT(vmcode_result == c_result, "VMCode and C versions have the "
                    "same result");

            IntRefVector_free_elems(args_to_c_version);
            IntRefVector_free_elems(initial_vm_stack);
        }

        ICodeOperationVector_free_elems(icodevec);
        ParseResult_free(pr);
    }

    destroy_testing_programs();

    END
END_TESTS
