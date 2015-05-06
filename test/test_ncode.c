#include <stdint.h>
#include "util.h"
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
                x86_64_preamble(NULL, &result),
                LOADIMM_to_x86_64(arg, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = (int (*)(void)) exec_mem;
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
                x86_64_preamble(NULL, &result),
                LOADIMM_to_x86_64(arg1, ACCUMULATOR),
                LOADIMM_to_x86_64(arg2, TEMPORARY),
                ADD_to_x86_64(TEMPORARY, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = (int (*)(void)) exec_mem;
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
                x86_64_preamble(NULL, &result),
                LOADIMM_to_x86_64(arg1, ACCUMULATOR),
                LOADIMM_to_x86_64(arg2, TEMPORARY),
                SUB_to_x86_64(ACCUMULATOR, TEMPORARY),
                HALT_to_x86_64
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = (int (*)(void)) exec_mem;
            func();
            ASSERT(result == arg1 - arg2,
                    "Subtraction yeilds correct result");
            release_executable(exec_mem, sizeof(code));
        }
    END

    TEST("Tests a load/store program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg = (rand() % 10000) - 5000;
            int result;
            byte *stack = challoc(sizeof(byte) * 1024);
            byte code[] = {
                x86_64_preamble(stack, &result),
                LOADIMM_to_x86_64(arg, TEMPORARY),
                STORE_to_x86_64(TEMPORARY, STACK_POINTER),
                LOAD_to_x86_64(STACK_POINTER, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem = allocate_executable(code, sizeof(code));
            int (*func)() = (int (*)(void)) exec_mem;
            func();
            ASSERT(result == arg,
                    "Load/store yeilds same value without error");
            release_executable(exec_mem, sizeof(code));
            free(stack);
        }
    END

    TEST("Tests a jump program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg = (rand() % 10000) - 5000;
            int result;
            byte *stack = challoc(sizeof(byte) * 1024);
            byte code2[] = {
                LOADIMM_to_x86_64(arg, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem2 = allocate_executable(code2, sizeof(code2));
            byte code1[] = {
                x86_64_preamble(stack, &result),
                JUMP_to_x86_64(exec_mem2)
            };
            byte *exec_mem1 = allocate_executable(code1, sizeof(code1));
            int (*func)() = (int (*)(void)) exec_mem1;
            func();
            ASSERT(result == arg,
                    "Jump occurs and control returns without error");
            release_executable(exec_mem1, sizeof(code1));
            release_executable(exec_mem2, sizeof(code2));
            free(stack);
        }
    END

    TEST("Tests a conditional jump program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg1 = (rand() % 10000) - 5000;
            int arg2 = (rand() % 10000) - 5000;
            int arg3 = (rand() % 10000) - 5000;
            int result;
            byte *stack = challoc(sizeof(byte) * 1024);
            byte code2[] = {
                LOADIMM_to_x86_64(arg2, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem2 = allocate_executable(code2, sizeof(code2));
            byte code1[] = {
                x86_64_preamble(stack, &result),
                LOADIMM_to_x86_64(arg1, ACCUMULATOR),
                JUMPCOND_to_x86_64(exec_mem2, ACCUMULATOR),
                LOADIMM_to_x86_64(arg3, ACCUMULATOR),
                HALT_to_x86_64
            };
            byte *exec_mem1 = allocate_executable(code1, sizeof(code1));
            int (*func)() = (int (*)(void)) exec_mem1;
            func();
            ASSERT(result == ((arg1 > 0) ? arg2 : arg3),
                    "Jump occurs if appropriate and control returns the correct"
                    " value without error");
            release_executable(exec_mem1, sizeof(code1));
            release_executable(exec_mem2, sizeof(code2));
            free(stack);
        }
    END

    TEST("Tests a call/return program returns correctly")
        int run;
        for(run = 0; run < 100; run++) {
            int arg = (rand() % 10000) - 5000;
            int result;
            byte *stack = challoc(sizeof(byte) * 1024);
            byte code2[] = {
                LOADIMM_to_x86_64(arg, ACCUMULATOR),
                JUMPADDR_to_x86_64(RETURN_ADDRESS)
            };
            byte *exec_mem2 = allocate_executable(code2, sizeof(code2));
            byte code1[] = {
                x86_64_preamble(stack, &result),
                JUMPLINK_to_x86_64(exec_mem2),
                HALT_to_x86_64
            };
            byte *exec_mem1 = allocate_executable(code1, sizeof(code1));
            int (*func)() = (int (*)(void)) exec_mem1;
            func();
            ASSERT(result == arg,
                    "Call and return occur with the correct value and no "
                    "errors");
            release_executable(exec_mem1, sizeof(code1));
            release_executable(exec_mem2, sizeof(code2));
            free(stack);
        }
    END
END_TESTS
