#ifndef util
#include "util.h"
#endif // util

#ifndef icode_interpreter
#define icode_interpreter

typedef struct ICodeInterpreterState ICodeInterpreterState;
struct ICodeInterpreterState {
    struct ICodeOperationVector *code;
    int *stack;
    int *registers;
    int *labels;
    int min_label;
};

int ICodeOperationVector_execute(ICodeOperationVector *code,
        IntRefVector *initial_stack);
ICodeInterpreterState *ICodeInterpreterState_init(ICodeOperationVector *code,
        IntRefVector *initial_stack);
void ICodeInterpreterState_free(ICodeInterpreterState *state);
int ICodeInterpreterState_result(ICodeInterpreterState *state);
void ICodeInterpreterState_step(ICodeInterpreterState *state);
void ICodeInterpreterState_run(ICodeInterpreterState *state);

#endif // icode_interpreter
