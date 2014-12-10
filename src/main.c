#include <stdio.h>
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

    // Try to open the file
	FILE *file = fopen(argv[1], "r");

    // If it couldn't be opened, quit with an appropriate error message
	if (!file) {
        printf("Could not open file: '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
	}

    LexerResult *lr = lex_file(file);
	TokenVector *tokens = LexerResult_tokens(lr);

    // If there are lexical errors, print them and quit
    if(!verify_lex(tokens)) {
        print_errors(tokens);
        LexerResult_free(lr);
        exit(EXIT_FAILURE);
    }

    Prog *prog = parse(tokens);
    Prog_print(prog, 0);

    LexerResult_free(lr);
    Prog_free(prog);

    return 0;
}
