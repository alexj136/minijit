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

ICodeInterpreterState *ICodeInterpreterState_init(ICodeOperationVector *code);
void ICodeInterpreterState_free(ICodeInterpreterState *state);
int ICodeInterpreterState_result(ICodeInterpreterState *state);
void prepare_state(ICodeInterpreterState *state, Prog *prog,
        IntRefVector *args);
void ICodeInterpreterState_step(ICodeInterpreterState *state);
void ICodeInterpreterState_run(ICodeInterpreterState *state);

#endif // icode_interpreter
