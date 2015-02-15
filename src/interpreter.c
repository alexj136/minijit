#include "interpreter.h"

/*
 * Store function definitions
 */

Store *Store_init(IntRefVector *argNames, IntRefVector *argValues) {

    if(IntRefVector_size(argNames) != IntRefVector_size(argValues)) {
        ERROR("Store creation with differing numbers of formal "
                "and actual parameters");
    }

    int idx;
    for(idx = 0; idx < IntRefVector_size(argNames); idx++) {
        NOT_IMPLEMENTED;
    }
}

/*
 * Does a binary search of a Store within the given start & end indexes,
 * returning the name's index in the list, or -1 if the name is not present.
 */
int Store_check_name_index(Store *store, IntRef *name) {

    int  store_size = IntRefVector_size(store);
    int  jump_size  = store_size / 2;
    int  idx        = store_size / 2;

    while(jump_size > 0) {

        // Look at the name in the current index
        int name_at_idx = IntRefVector_get(store->names, idx);

        // Decrease the jump size. This must be done before the jump with this
        // algorithm. Otherwise, we'll quit the loop before we inspect the last
        // element.
        jump_size /= 2;

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
    }

    // Didn't find, so return -1.
    return -1;
}

void Store_assign(Store *store, IntRef *name, int value) {

    int idx_of_var = Store_check_name_index(store, name);

    // Variable not in the store, so we must create it
    if(idx_of_var < 0) {

        // Iterate to find the insertion index
        int idx;
        for(idx = 0; idx < IntRefVector_size(store->names); idx++) {

            if(IntRef_value(name) < IntRefVector_get(store->names, idx)) {
                break;
            }

            if(IntRef_value(name) == IntRefVector_get(store->names, idx)) {
                ERROR("Binary search told us that variable was not present in "
                        "store, however linear search told us there was.");
            }

            else { idx++; }
        }

        // Now we've found the appropriate index for insertion, simply insert
        // the name and value at that location using vector functions.
        IntRefVector_insert(store->names, idx, name);
        IntRefVector_insert(store->values, idx, IntRef_init(value));

    }

    // Variable already in the store, just update it
    else {
        IntRef_value(IntRefVector_get(store->values, idx_of_var)) = value;
    }
}

/*
 * Look up a variable value in the store. The semantics of the language are such
 * that currently unassigned variables have value 0, so if a non-stored name is
 * queried, we just return 0 as its value.
 */
int Store_lookup(Store *store, IntRef *name) {

    int idx_of_var = Store_check_name_index(store, name);

    if(idx_of_var < 0) {
        // The semantics of the language are such that all variables have value
        // 0 until otherwise assigned
        return 0;
    }

    else {
        return IntRef_value(IntRefVector_get(store->values, idx_of_var));
    }
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
