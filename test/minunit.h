#ifndef MINUNIT
#define MINUNIT

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define MINUNIT_TESTS \
    int main(int argc, char *argv[]) { \
        int __passes__ = 0, __fails__ = 0, __asserts__ = 0;
#define END_TESTS \
        printf("%s: Ran %d test%s with %d pass%s and %d failure%s. ", \
                argv[0], \
                __passes__ + __fails__, \
                (__passes__ + __fails__ == 1) ? "" : "s", \
                __passes__, (__passes__ == 1) ? "" : "es", \
                __fails__, \
                (__fails__ == 1) ? "" : "s"); \
        printf("%d assertions run.\n", __asserts__); \
        return __fails__ ? EXIT_FAILURE : EXIT_SUCCESS; \
    }

#define TEST(desc) \
    do { \
        char *__desc__ = desc; \
        bool __passed__ = true, __done_assert__ = false;
#define END \
        if(!__done_assert__) { \
            printf("Warning: Test '%s' contains no assertions.\n", __desc__); \
        } \
        if(__passed__) { __passes__++; } \
        else { \
            __fails__++; \
            printf("Test '%s' failed.\n", __desc__); \
        } \
    } while(0);

#define ASSERT(test, message) \
	if (!(test)) { \
        __passed__ = false; \
		printf("Assertion '%s' failed.\n", message); \
	} \
    else { __asserts__++; __done_assert__ = true; }

#endif // MINUNIT
