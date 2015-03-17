#ifndef icodegen
#define icodegen

OperationVector *icodegen_Prog(Prog *prog, int *next_label);
OperationVector *icodegen_Func(Prog *prog, Func *func, int *next_label,
        int *func_labels);
OperationVector *icodegen_Comm(Prog *prog, Comm *comm, int *next_label,
        int *func_labels);
OperationVector *icodegen_Expr(Prog *prog, Expr *expr, int *next_label,
        int *func_labels);

#endif // icodegen
