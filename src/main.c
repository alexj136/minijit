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

	TokenVector *tokens = lex_file(argv[1]);
    bool safe = verify_lex(tokens);
    if(!safe) {
        TokenVector_free_elems(tokens);
        puts("Lexical error(s) found. Exiting.");
        exit(EXIT_FAILURE);
    }

    Comm *comm = parse(tokens);
    Comm_print(comm, 0);

    TokenVector_free_elems(tokens);
    Comm_free(comm);

    return 0;
}
