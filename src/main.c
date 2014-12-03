#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {

    // Lex and parse the given file and print the results

    if(argc < 2) {
        puts("No arguments provided.");
        exit(EXIT_FAILURE);
    }

	Token **tokens = lex_file(argv[1]);
    bool safe = verify_lex(tokens);
    if(!safe) {
        Token_arr_free(tokens);
        puts("Lexical error(s) found. Exiting.");
        exit(EXIT_FAILURE);
    }

    Comm *comm = parse(tokens);
    Comm_print(comm, 0);

    Token_arr_free(tokens);
    Comm_free(comm);

    return 0;
}
