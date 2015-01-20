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

    // Lex the file
    LexerResult *lr = lex_file(file);

    // Close the file
    int close_status = fclose(file);
    if(close_status != 0) {
        printf("Warning: file '%s' did not close properly.\n", argv[1]);
    }

	TokenVector *tokens = LexerResult_tokens(lr);

    // If there are lexical errors, print them and quit
    if(!verify_lex(tokens)) {
        print_errors(tokens);
        LexerResult_free(lr);
        exit(EXIT_FAILURE);
    }

    Prog *prog = parse(tokens);
    Prog_print(prog, 0, LexerResult_name_map(lr));

    LexerResult_free(lr);
    Prog_free(prog);

    return 0;
}
