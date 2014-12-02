#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"

int main(int argc, char *argv[]) {

    // f(a, b) { return f(10, 10); }
    // g() { return 1; }

    Expr **args = challoc(sizeof(Expr*) * 2);
    Expr *call = Call_init(0, 2, args);
    Call_arg(call, 0) = Int_init(10);
    Call_arg(call, 1) = Int_init(11);

    Func **funcs = challoc(sizeof(Func*) * 2);
    Prog *prog = Prog_init(2, funcs);
    Prog_func(prog, 0) = Func_init(0, 2, Return_init(call));
    Prog_func(prog, 1) = Func_init(1, 0, Return_init(Int_init(1)));

    Prog_print(prog, 0);
    Prog_free(prog);

    puts("===================================================================");

    // Lex a given file and print the results

    if(argc < 2) {
        puts("No arguments provided.");
        exit(EXIT_FAILURE);
    }

	Token **tokens = lex_file(argv[1]);
    int idx = 0;
    while(Token_type(tokens[idx]) != endOfInput) {
        Token_print(tokens[idx]);
        idx++;
    }
    Token_arr_free(tokens);
    return 0;
}
