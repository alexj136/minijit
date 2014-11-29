#include <malloc.h>
#include "util.h"
#include "syntax.h"

int main(int argc, char *argv[]) {

    // f(a, b) { return f(10, 10); }

    Expr **args = challoc(sizeof(Expr*) * 2);
    Expr *call = Call_init(0, 2, args);
    Call_arg(call, 0) = Int_init(10);
    Call_arg(call, 1) = Int_init(11);
    Expr_print(*args, 0);
    Expr_print(*(args + sizeof(Expr*)), 0);

    Func **funcs = challoc(sizeof(Func*));
    *funcs = Func_init(0, 2, Return_init(call));
    Prog *prog = Prog_init(1, funcs);

    Prog_print(prog, 0);
    Prog_free(prog);
    return 0;
}
