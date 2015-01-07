#ifndef MINUNIT
#define MINUNIT

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define MINUNIT_TESTS \
    int main(int argc, char *argv[]) { \
        int ___passes = 0, ___fails = 0, ___asserts = 0;
#define END_TESTS \
        int ___total = ___passes + ___fails; \
        printf("%s: Ran %d test%s with %d pass%s and %d failure%s. ", \
                argv[0], \
                ___total, \
                (___total == 1) ? "" : "s", \
                ___passes, (___passes == 1) ? "" : "es", \
                ___fails, \
                (___fails == 1) ? "" : "s"); \
        printf("%d assertions run.\n", ___asserts); \
        if(___total < 1) { \
            printf("Warning: File '%s' contains no tests.\n", __FILE__); \
        } \
        return ___fails ? EXIT_FAILURE : EXIT_SUCCESS; \
    }

#define TEST(desc) \
    do { \
        char *___desc = desc; \
        bool ___passed = true, ___done_assert = false;
#define END \
        if(!___done_assert) { \
            printf("Warning: Test '%s' contains no assertions.\n", ___desc); \
        } \
        if(___passed) { ___passes++; } \
        else { \
            ___fails++; \
            printf("Test '%s' failed.\n", ___desc); \
        } \
    } while(0);

#define ASSERT(test, message) \
	if (!(test)) { \
        ___passed = false; \
        ___done_assert = true; \
		printf("Assertion '%s' failed.\n", message); \
	} \
    else { ___asserts++; ___done_assert = true; }

#endif // MINUNIT
