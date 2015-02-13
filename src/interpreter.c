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
int Store_check_name_index(Store *store, IntRef *name) {

    int  store_size = IntRefVector_size(store);
    int  jump_size  = store_size / 4;
    int  idx        = store_size / 2;

    while(jump_size > 0) {

        int name_at_idx = IntRefVector_get(store->names, idx);

        if(name == name_at_idx) {
            // Found - return the index.
            return idx;
        }

        else if(name < name_at_idx) {
            // Name at index was greater than the name we're looking for.
            // Subtract from the index.
            idx -= jump_size;
        }

        else /* name > name_at_idx */ {
            // Name at index was less than the name we're looking for.
            // Add to the index.
            idx += jump_size;
        }

        jump_size /= 2;
    }

    // Didn't find, so return -1.
    return -1;
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
