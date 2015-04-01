#ifndef util
#include "util.h"
#endif // util

#ifndef testing_programs
#define testing_programs

typedef struct TestProgram TestProgram;
struct TestProgram {
    char *prog_str; // Assumed to be static data
    int arity;
    int (*c_version)(IntRefVector *);
};

FORWARD_DECLARE_VECTORABLE(TestProgram)

TestProgram *TestProgram_init(char *prog_str, int arity, int (*c_version)(IntRefVector *));
bool TestProgram_eq(TestProgram *tp1, TestProgram *tp2);
void TestProgram_free(TestProgram *tp);

void prepare_testing_programs();
TestProgram *get_test_program(int number);
void destroy_testing_programs();

#endif // testing_programs
