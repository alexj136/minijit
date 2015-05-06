#ifndef MINUNIT
#define MINUNIT

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define MINUNIT_TESTS \
    int main(int argc, char *argv[]) { \
        int ___test_passes = 0, ___test_fails = 0, ___assert_passes = 0, \
                ___assert_fails = 0; \
        argc = argc; /* supress warnings about argc being unused */
#define END_TESTS \
        int ___total = ___test_passes + ___test_fails; \
        printf("%s: Ran %d test%s with %d pass%s and %d failure%s. ", \
                argv[0], \
                ___total, \
                (___total == 1) ? "" : "s", \
                ___test_passes, (___test_passes == 1) ? "" : "es", \
                ___test_fails, \
                (___test_fails == 1) ? "" : "s"); \
        printf("%d assertions run, %d of which did not hold.\n", \
                ___assert_passes + ___assert_fails, ___assert_fails); \
        if(___total < 1) { \
            printf("Warning: File '%s' contains no tests.\n", __FILE__); \
        } \
        return ___test_fails ? EXIT_FAILURE : EXIT_SUCCESS; \
    }

#define TEST(desc) \
    do { \
        char *___desc = desc; \
        bool ___test_passed = true, ___done_assert = false;
#define END \
        if(!___done_assert) { \
            printf("Warning: Test '%s' contains no assertions.\n", ___desc); \
        } \
        if(___test_passed) { ___test_passes++; } \
        else { \
            ___test_fails++; \
            printf("Test '%s' failed.\n", ___desc); \
        } \
    } while(0);

#define ASSERT(test, message) \
	if (!(test)) { \
        ___test_passed = false; \
        ___done_assert = true; \
        ___assert_fails++; \
		printf("%s:%d - Assertion failed:\n", __FILE__, __LINE__); \
		printf("    %s\n", message); \
	} \
    else { ___assert_passes++; ___done_assert = true; }

#endif // MINUNIT
