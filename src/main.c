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

    LexerResult *lr = lex_file(argv[1]);
	TokenVector *tokens = LexerResult_tokens(lr);

    int idx;
    for(idx = 0; idx < TokenVector_size(tokens); idx++) {
        Token_print(TokenVector_get(tokens, idx));
    }

    bool safe = verify_lex(tokens);
    if(!safe) {
        TokenVector_free_elems(tokens);
        puts("Lexical error(s) found. Exiting.");
        exit(EXIT_FAILURE);
    }

    Prog *prog = parse(tokens);
    Prog_print(prog, 0);

    LexerResult_free(lr);
    Prog_free(prog);

    return 0;
}
