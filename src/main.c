#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "util.h"
#include "syntax.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char *argv[]) {

    // Lex and parse the given file and print the results

    if(argc < 2) {
        puts("No arguments provided.");
        exit(EXIT_FAILURE);
    }

    // Parse the command line arguments
    int *prog_args    = challoc(sizeof(int) * (argc - 2));
    int prog_args_idx = 0;
    int argv_idx      = 2;
    bool no_errors    = true;
    while((argv_idx < argc) && no_errors) {

        IntRef *arg_result = str_to_int(argv[argv_idx]);

        if(arg_result == NULL) {
            no_errors = false;
        }
        else {
            prog_args[prog_args_idx] = IntRef_value(arg_result);
        }

        prog_args_idx++;
        argv_idx++;
    }

    // If argument parsing failed, quit with an appropriate error message
    if(!no_errors) {
        free(prog_args);
        printf("Could not parse command line arguments.");
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

    // If there are lexical errors, print them and quit
    if(!verify_lex(lr)) {
        print_errors(lr);
        LexerResult_free(lr);
        exit(EXIT_FAILURE);
    }

    // Do the parsing of the source file
    Prog *prog = parse(lr);

    // Check that we got the appropriate number of arguments on the command line
    if(argc - 2 != Prog_num_args(prog)) {
        printf("main expects %d arguments, but %d were given.\n",
                Prog_num_args(prog), argc - 2);

        Prog_free(prog);
        free(prog_args);
        exit(EXIT_FAILURE);
    }

    // Run the interpreter
    InterpretResult *res = interpret_Prog(prog, prog_args);
    if(res->type != iSuccess) {
        puts("Runtime error.");
    }
    else {
        printf("RESULT = %d\n", res->result);
    }

    // Free things
    free(res);
    Prog_free(prog);
    free(prog_args);

    return 0;
}
