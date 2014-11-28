#include "util.h"
#include "syntax.h"

int main(int argc, char *argv[]) {

    Expr *a = Int_init(10);
    Expr_free(a);
    return 0;
}
