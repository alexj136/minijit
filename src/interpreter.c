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

/*
 * Does a binary search of a Store within the given start & end indexes,
 * returning the name's index in the list, or -1 if the name is not present.
 */
int *Store_check_name_index(Store *store, IntRef *name, int startIdx,
        int endIdx) {

    if(startIdx == endIdx) {
        // if name here matches return the value, otherwise return -1
    }
    else {
        // Make a recursive call
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
