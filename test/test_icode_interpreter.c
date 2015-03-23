#include "minunit.h"
#include "syntax.h"
#include "icode.h"
#include "icode_interpreter.h"

MINUNIT_TESTS

    TEST("Very simple icode program")

        ICodeOperationVector *code = ICodeOperationVector_init();
        ICodeOperationVector_append(code,
                ICodeOperation_init(LOADIMM, 3, ACCUMULATOR));
        ICodeOperationVector_append(code,
                ICodeOperation_init(HALT, 0, 0));
        IntRefVector *initial_stack = IntRefVector_init();

        ASSERT(ICodeOperationVector_execute(code, initial_stack) == 3,
                "Result is correct");

        IntRefVector_free_elems(initial_stack);
        ICodeOperationVector_free_elems(code);
    END

END_TESTS
