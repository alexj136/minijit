#include <stdint.h>
#include "minunit.h"
#include "icode.h"
#include "ncode.h"

MINUNIT_TESTS

    TEST("Tests a LOADIMM 10; HALT ncode program returns correctly")
        int result;
        byte code[] = {
            LOADIMM_to_x86_64(10, ACCUMULATOR),
            HALT_to_x86_64(&result)
        };
        byte *exec_mem = allocate_executable(code, sizeof(code));
        int (*func)() = exec_mem;
        func();
        ASSERT(result = 10, "LOADIMM 10; HALT yeilds correct result");
        release_executable(exec_mem, sizeof(code));
    END
END_TESTS
