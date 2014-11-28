#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include "util.h"

void *challoc(int size) {
	void *ptr = malloc(size);
	assert(ptr);
	return ptr;
}

void put_indent(int num) {
    int indents_done;
    for(indents_done = 0; indents_done < num; indents_done++) {
        printf("    ");
    }
    return;
}
