#include <stdint.h>
#include "minunit.h"
#include "icode.h"
#include "ncode.h"

MINUNIT_TESTS

    TEST("Tests a LOADIMM 10; HALT ncode program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg = (rand() % 10000) - 5000;
            int result;
            byte code[] = {
                LOADIMM_to_x86_64(arg, ACCUMULATOR),
                HALT_to_x86_64(&result)
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = exec_mem;
            func();
            ASSERT(result == arg, "LOADIMM 10; HALT yeilds correct result");
            release_executable(exec_mem, sizeof(code));
        }
    END

    TEST("Tests an addition program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg1 = (rand() % 10000) - 5000;
            int arg2 = (rand() % 10000) - 5000;
            int result;
            byte code[] = {
                LOADIMM_to_x86_64(arg1, ACCUMULATOR),
                LOADIMM_to_x86_64(arg2, TEMPORARY),
                ADD_to_x86_64(TEMPORARY, ACCUMULATOR),
                HALT_to_x86_64(&result)
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = exec_mem;
            func();
            ASSERT(result == arg1 + arg2,
                    "Addition yeilds correct result");
            release_executable(exec_mem, sizeof(code));
        }
    END

    TEST("Tests a subtraction program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg1 = (rand() % 10000) - 5000;
            int arg2 = (rand() % 10000) - 5000;
            int result;
            byte code[] = {
                LOADIMM_to_x86_64(arg1, ACCUMULATOR),
                LOADIMM_to_x86_64(arg2, TEMPORARY),
                SUB_to_x86_64(ACCUMULATOR, TEMPORARY),
                HALT_to_x86_64(&result)
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = exec_mem;
            func();
            ASSERT(result == arg1 - arg2,
                    "Subtraction yeilds correct result");
            release_executable(exec_mem, sizeof(code));
        }
    END
END_TESTS
