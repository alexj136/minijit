#ifndef icodegen
#define icodegen

ICodeOperationVector *icodegen_Prog(Prog *prog, int *next_label);
ICodeOperationVector *icodegen_Func(Prog *prog, Func *func, int *next_label,
        int *func_labels);
ICodeOperationVector *icodegen_Comm(Prog *prog, Comm *comm, int *next_label,
        int *func_labels);
ICodeOperationVector *icodegen_Expr(Prog *prog, Expr *expr, int *next_label,
        int *func_labels);

#endif // icodegen
