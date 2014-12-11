#ifndef MINUNIT
#define MINUNIT

#include <stdio.h>
#include <stdlib.h>

#define bool int
#define false 0
#define true 1

#define DECLARE_MINUNIT_MODULE \
    void __do_initial_setup__(); \
    void __do_setup__(); \
    void __do_teardown__(); \
    void __do_final_teardown__(); \

#define INITIAL_SETUP void __do_initial_setup__() {
#define END_INITIAL_SETUP }
#define SETUP void __do_setup__() {
#define END_SETUP }
#define TEARDOWN void __do_teardown__() {
#define END_TEARDOWN }
#define FINAL_TEARDOWN void __do_final_teardown__() {
#define END_FINAL_TEARDOWN }

#define BEGIN_TESTS \
    int main(int argc, char *argv[]) { \
        int __passes__ = 0, __fails__ = 0; \
        __do_initial_setup__();
#define END_TESTS \
        __do_final_teardown__(); \
        printf("%s: Ran %d tests with %d passes and %d failures.\n", argv[0], \
                __passes__ + __fails__, __passes__, __fails__); \
        return __fails__ ? EXIT_FAILURE : EXIT_SUCCESS; \
    }

#define TEST(desc) \
    do { \
        char *__desc__ = desc; \
        bool __passed__ = true; \
        __do_setup__();
#define END \
        if(__passed__) { __passes__++; } \
        else { \
            __fails__++; \
            printf("Test '%s' failed.\n", __desc__); \
        } \
        __do_teardown__(); \
    } while(0);

#define ASSERT(test, message) \
	if (!(test)) { \
        __passed__ = false; \
		printf("Assertion '%s' failed.\n", message); \
	}

#endif // MINUNIT
