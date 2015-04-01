#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "testing_programs.h"

DEFINE_VECTORABLE(TestProgram)

TestProgram *TestProgram_init(char *prog_str, int arity,
        int (*c_version)(IntRefVector *)) {

    TestProgram *tp = challoc(sizeof(TestProgram));
    tp->prog_str    = prog_str;
    tp->arity       = arity;
    tp->c_version   = c_version;
    return tp;
}

bool TestProgram_eq(TestProgram *tp1, TestProgram *tp2) {
    if(!tp1) { ERROR("Null TestProgram"); }
    if(!tp2) { ERROR("Null TestProgram"); }
    ERROR("Equality not defined for TestPrograms");
}

void TestProgram_free(TestProgram *tp) {
    // The prog_str is static data, don't free this
    // The c_version function is code, don't free this
    free(tp);
}

/*******************************************************************************
 * TEST PROGRAM DECLARATIONS AND THEIR C VERSIONS                              *
 ******************************************************************************/

// Super simple program that just returns an integer literal
char *tp0_str = "main() { return 10 }";
int tp0_arity = 0;
int tp0_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp0_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    return 10;
}


// Super simple program that just returns its single argument
char *tp1_str = "main(a) { return a }";
int tp1_arity = 1;
int tp1_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp1_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    return a;
}

// Simple program that just returns the sum of its two arguments
char *tp2_str = "main(a, b) { return a + b }";
int tp2_arity = 2;
int tp2_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp2_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    int b = IntRef_value(IntRefVector_get(args, 1));
    return a + b;
}

// Simple program that just returns the subtraction of its second argument
// from the first
char *tp3_str = "main(a, b) { return a - b }";
int tp3_arity = 2;
int tp3_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp3_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    int b = IntRef_value(IntRefVector_get(args, 1));
    return a - b;
}

// Simple program that reassigns its argument to be one greater than its
// initial value, and then returns it
char *tp4_str = "main(a) { a := a + 1; return a }";
int tp4_arity = 1;
int tp4_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp4_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    a = a + 1;
    return a;
}

// Program that computes the product of its arguments, given that they are
// both greater than or equal to 1
char *tp5_str = "  main(a, b) {            "
                "      c := a;             "
                "      while b - 1 do {    "
                "          b := b - 1;     "
                "          a := a + c      "
                "      };                  "
                "      return a            "
                "  }                       ";
int tp5_arity = 2;
int tp5_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp5_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    int b = IntRef_value(IntRefVector_get(args, 1));
    int c = a;
    while((b - 1) > 0) {
        b = b - 1;
        a = a + c;
    }
    return a;
}

// Program that does a simple function call
char *tp6_str = "  main(a) {               "
                "      return foo(a) + a   "
                "  }                       "
                "  foo(a) {                "
                "      return a + 10       "
                "  }                       ";
int tp6_arity = 1;
int tp6_foo(int a) {
    return a + 10;
}
int tp6_main(IntRefVector *args) {
    if(IntRefVector_size(args) != tp6_arity) {
        ERROR("Incorrect number of arguments given to a test program");
    }
    int a = IntRef_value(IntRefVector_get(args, 0));
    return tp6_foo(a) + a;
}

/*******************************************************************************
 * END OF TEST PROGRAM DECLARATIONS AND THEIR C VERSIONS                       *
 ******************************************************************************/

TestProgramVector *test_program_vector = NULL;

void prepare_testing_programs() {
    // Quit if the test_program_vector has already been prepared
    if(test_program_vector) { ERROR("Testing programs already prepared"); }
    test_program_vector = TestProgramVector_init();
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp0_str, tp0_arity, &tp0_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp1_str, tp1_arity, &tp1_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp2_str, tp2_arity, &tp2_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp3_str, tp3_arity, &tp3_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp4_str, tp4_arity, &tp4_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp5_str, tp5_arity, &tp5_main));
    TestProgramVector_append(test_program_vector,
            TestProgram_init(tp6_str, tp6_arity, &tp6_main));
}

TestProgram *get_test_program(int number) {
    if(!test_program_vector) { ERROR("Testing programs not prepared"); }

    if(number >= TestProgramVector_size(test_program_vector)) {
        ERROR("test program at given index does not exist");
    }
    else { return TestProgramVector_get(test_program_vector, number); }
}

int num_testing_programs() {
    if(!test_program_vector) { ERROR("Testing programs not prepared"); }
    return TestProgramVector_size(test_program_vector);
}

void destroy_testing_programs() {
    if(!test_program_vector) { ERROR("Testing programs not prepared"); }
    TestProgramVector_free_elems(test_program_vector);
}
