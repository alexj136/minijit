#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include "util.h"

/*
 * Perform checked heap allocation.
 */
void *challoc(int size) {
	void *ptr = malloc(size);
    if(!ptr) {
        puts("util.c/challoc(): Heap allocation failed");
        exit(EXIT_FAILURE);
    }
	return ptr;
}

/*
 * Perform checked reallocation.
 */
void *chrealloc(void *ptr, int size) {
	void *new = realloc(ptr, size);
    if(!new) {
        puts("util.c/chrealloc(): Heap reallocation failed");
        exit(EXIT_FAILURE);
    }
	return new;
}

/*
 * Print the given number of tabs to stdout, without newlines.
 */
void put_indent(int num) {
    int indents_done;
    for(indents_done = 0; indents_done < num; indents_done++) {
        printf("    ");
    }
    return;
}
