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
    int next_label;
    int *labels;
};

ICodeInterpreterState *ICodeInterpreterState_init(ICodeOperationVector *code,
        int stack_size, int next_reg, int next_label);
void prepare_stack(ICodeInterpreterState *state, Prog *prog,
        IntRefVector *args);
void ICodeInterpreterState_step(ICodeInterpreterState *state);
void ICodeInterpreterState_run(ICodeInterpreterState *state);

#endif // icode_interpreter
