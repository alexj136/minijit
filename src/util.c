#include <assert.h>
#include "util.h"

void *challoc(int size) {
	void *ptr = malloc(size);
	assert(ptr);
	return ptr;
}
