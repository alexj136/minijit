#ifndef util
#include "util.h"
#endif // util

#ifndef vmcode_interpreter
#define vmcode_interpreter

typedef struct VMCodeInterpreterState VMCodeInterpreterState;
struct VMCodeInterpreterState {
    int *code;
    int *stack;
    int *registers;
    int *labels;
    int min_label;
};

VMCodeInterpreterState *VMCodeInterpreterState_init(
        ICodeOperationVector *code, IntRefVector *initial_stack);
void VMCode_run(VMCodeInterpreterState *state);

#endif // vmcode_interpreter
