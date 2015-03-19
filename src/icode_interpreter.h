#ifndef util
#include "util.h"
#endif // util

#ifndef icode_interpreter
#define icode_interpreter

typedef struct ICodeInterpreterState ICodeInterpreterState;
struct ICodeInterpreterState {
    struct ICodeOperationVector *code;
    int stack_size;
    int *stack;
    int next_reg;
    int *registers;
    int *labels;
};

void ICodeInterpreterState_step(ICodeInterpreterState *state);
void ICodeInterpreterState_run(ICodeInterpreterState *state);

#endif // icode_interpreter
