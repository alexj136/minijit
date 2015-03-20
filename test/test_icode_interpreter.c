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

        ICodeInterpreterState *state =
                ICodeInterpreterState_init(code, 0, TEMPORARY + 1, 0);

        ICodeInterpreterState_run(state);
        ASSERT(ICodeInterpreterState_result(state) == 3, "Correct result");

        ICodeInterpreterState_free(state);
        ICodeOperationVector_free_elems(code);
    END

END_TESTS
