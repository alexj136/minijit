#ifndef icodegen
#define icodegen

OperationVector *icodegen_Prog(Prog *prog, int *next_label);
OperationVector *icodegen_Func(Func *func, int *next_label);
OperationVector *icodegen_Comm(Comm *comm, int *next_label);
OperationVector *icodegen_Expr(Expr *expr, int *next_label);

#endif // icodegen
