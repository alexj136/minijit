#include "interpreter.h"

/*
 * Store function definitions
 */

Store *Store_init(IntRefVector *argNames, IntRefVector *argValues) {

    if(IntRefVector_size(argNames) != IntRefVector_size(argValues)) {
        ERROR("Store creation with differing numbers of formal "
                "and actual parameters");
    }

    NOT_IMPLEMENTED;
}

void Store_assign(Store *store, IntRef *name, int value) {
    NOT_IMPLEMENTED;
}

int Store_lookup(Store *store, IntRef *name) {
    NOT_IMPLEMENTED;
}

/*
 * Interpreter function definitions
 */

InterpretResult *interpret_Prog(Prog *prog, int *args) {
    NOT_IMPLEMENTED;
}

InterpretResult *interpret_Func(Func *func, int *args) {
    NOT_IMPLEMENTED;
}

InterpretResult *interpret_Comm(Comm *comm, Store *store) {
    NOT_IMPLEMENTED;
}

InterpretResult *interpret_Expr(Expr *expr, Store *store) {
    NOT_IMPLEMENTED;
}
