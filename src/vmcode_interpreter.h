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

int VMCode_execute(ICodeOperationVector *code, IntRefVector *initial_stack);
VMCodeInterpreterState *VMCodeInterpreterState_init(
        ICodeOperationVector *code, IntRefVector *initial_stack);
void VMCodeInterpreterState_free(VMCodeInterpreterState *state);
void VMCodeInterpreterState_run(VMCodeInterpreterState *state);

#endif // vmcode_interpreter
