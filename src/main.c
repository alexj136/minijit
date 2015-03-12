#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

/*
 * Proceeds through the interpreter pipeline, checking the output of each stage
 * for errors, and halting early if necessary.
 */
int main(int argc, char *argv[]) {

    /* =========================================================================
     * Check that an argument file was supplied. Quit if not.
     */
    if(argc < 2) {
        puts("No arguments provided.");
        exit(EXIT_FAILURE);
    }

    /* =========================================================================
     * We now know a file argument was supplied - try to open it, and quit if it
     * fails.
     */
	FILE *file = fopen(argv[1], "r");

	if (!file) {
        printf("Could not open file: '%s'.\n", argv[1]);
        exit(EXIT_FAILURE);
	}

    /* =========================================================================
     * We now know that the file opened successfully, so perform lexical
     * analysis on its contents.
     */
    LexerResult *lr = lex_file(file);

    /* =========================================================================
     * Close the file now that lexical analysis has concluded. Print a warning
     * if closing fails, but continue with the pipeline anyway.
     */
    int close_status = fclose(file);
    if(close_status != 0) {
        printf("Warning: file '%s' did not close properly.\n", argv[1]);
    }

    /* =========================================================================
     * Do the parse. We don't need to check if the lexer failed because the
     * parser can deal with token lists that contain errors, and return any
     * lexical errors along with parse errors.
     *     If parsing fails, print the errors and exit.
     */
    ParseResult *pr = parse(lr);

    if(pr->type == parseFail) {
        ParseErrorVector_print(pr->errors);
        ParseResult_free(pr);
        exit(EXIT_FAILURE);
    }

    /* =========================================================================
     * We got the correct number of arguments, so now parse the command-line
     * arguments. If parsing fails, quit with an appropriate error message.
     */
    IntRefVector *prog_args = IntRefVector_init();
    int argv_idx      = 2;
    bool no_errors    = true;
    while((argv_idx < argc) && no_errors) {

        IntRef *arg_result = str_to_int(argv[argv_idx]);

        if(arg_result == NULL) {
            no_errors = false;
        }
        else {
            IntRefVector_append(prog_args, arg_result);
        }

        argv_idx++;
    }

    if(!no_errors) {
        IntRefVector_free_elems(prog_args);
        ParseResult_free(pr);
        printf("Could not parse command line arguments.\n");
        exit(EXIT_FAILURE);
    }

    /* =========================================================================
     * We now have everything we need to run the program, so run the program and
     * print the result.
     */
    InterpretResult *res = interpret_Prog(pr->prog, prog_args);
    if(res->type == iFailIncorrectNumArgs) {
        puts("Runtime Error: Function call with incorrect number of "
                "arguments.");
    }
    else if(res->type == iFailFunctionNotFound) {
        puts("Runtime Error: Call to an undefined function.");
    }
    else if(res->type == iFailEndWithoutReturn) {
        puts("Runtime Error: Reached end of function without return.");
    }
    else if(res->type == iSuccess) {
        printf("RESULT = %d\n", res->result);
    }
    else {
        free(res);
        ParseResult_free(pr);
        IntRefVector_free_elems(prog_args);
        ERROR("InterpretResult type not recognised");
    }

    /* =========================================================================
     * Deallocate the remaining heap objects.
     */
    Prog_print(pr->prog, 0);
    free(res);
    ParseResult_free(pr);
    IntRefVector_free_elems(prog_args);

    return 0;
}
