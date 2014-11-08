#include <assert.h>
#include <malloc.h>
#include "util.h"

void *challoc(int size) {
	void *ptr = malloc(size);
	assert(ptr);
	return ptr;
}
