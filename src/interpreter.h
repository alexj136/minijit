#ifndef interpreter
#define interpreter

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

InterpretResult *interpret_Prog(Prog *prog, int *args);

#endif // interpreter
