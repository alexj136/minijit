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
    iFailCouldNotParseArgs,
    iFailFunctionNotFound,
    iFailEndWithoutReturn
} interpretResultType;

typedef struct InterpretResult InterpretResult;
struct InterpretResult {
    interpretResultType type;
    int result;
    bool returning;
    int line_no;
    int char_no;
};

InterpretResult *InterpretSuccess_init(int result);
InterpretResult *InterpretFailIncorrectNumArgs_init(int line_no, int char_no);
InterpretResult *InterpretFailCouldNotParseArgs_init(int line_no, int char_no);
InterpretResult *InterpretFailFunctionNotFound_init(int line_no, int char_no);
InterpretResult *InterpretFailEndWithoutReturn_init(int line_no, int char_no);

/*
 * A store contains two Vectors of integer references. The first is a list of
 * bound names. This list corresponds to the second list, which contains the
 * values of those bound names. Since these lists are indexable, variable lookup
 * and reassignment is O(log n) with binary search, and initial assignment is
 * O(n), where n is the number of variables in the store (i.e. in scope in the
 * function).
 */

InterpretResult *interpret_Prog(Prog *prog, int *args);
InterpretResult *interpret_Func(Prog *prog, Func *func, int *args);
InterpretResult *interpret_Comm(Prog *prog, Comm *comm, int *store);
InterpretResult *interpret_Expr(Prog *prog, Expr *expr, int *store);

#endif // interpreter
