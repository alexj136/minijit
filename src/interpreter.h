#ifndef interpreter
#define interpreter

/*
 * An InterpretResult contains two integer values - the result value and the
 * interpreter status code (either success, failure due to incorrect number of
 * arguments, or failure because the arguments could not be parsed).
 * InterpretResult structs also contain a string field for error messages.
 */

typedef enum {
    iSuccess,
    iFailIncorrectNumArgs,
    iFailCouldNotParseArgs
} interpretResultType;

typedef struct InterpretResult InterpretResult;
struct InterpretResult {
    interpretResultType type;
    int result;
    char *msg;
};

/*
 * A store contains two Vectors of integer references. The first is a list of
 * bound names. This list corresponds to the second list, which contains the
 * values of those bound names. Since these lists are indexable, variable lookup
 * and reassignment is O(log n) with binary search, and initial assignment is
 * O(n), where n is the number of variables in the store (i.e. in scope in the
 * function).
 */

typedef struct Store Store;
struct Store {
    struct IntRefVector *names;
    struct IntRefVector *values;
};

Store *Store_init();

InterpretResult *interpret_Prog(Prog *prog, int *args);
InterpretResult *interpret_Func(Func *func, int *args);
InterpretResult *interpret_Comm(Comm *comm, Store *store);
InterpretResult *interpret_Expr(Expr *expr, Store *store);

#endif // interpreter
