#include <malloc.h>
#include "util.h"
#include "syntax.h"

int main(int argc, char *argv[]) {

    // f(a, b) { return f(10, 10); }

    Expr **args = challoc(sizeof(Expr) * 2);
    *args = Int_init(10);
    *(args + sizeof(Expr)) = Int_init(10);
    Expr_free(*args);
    Expr_free(*(args + sizeof(Expr)));
    free(args);

    Func **funcs = challoc(sizeof(Func));
    *funcs = Func_init(0, 2, Return_init(Call_init(0, 2, args)));
    Prog *prog = Prog_init(1, funcs);

    Prog_print(prog, 0);
    Prog_free(prog);
    return 0;
}
