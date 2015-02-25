#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include "util.h"

/*
 * Perform checked heap allocation.
 */
void *challoc(int size) {
	void *ptr = malloc(size);
    if(!ptr) { ERROR("heap allocation failed"); }
	return ptr;
}

/*
 * Perform checked reallocation.
 */
void *chrealloc(void *ptr, int size) {
	void *new = realloc(ptr, size);
    if(!new) { ERROR("heap reallocation failed"); }
	return new;
}

/*
 * Copy a string and check for a null pointer.
 */
char *chstrdup(char *str) {
    char *new_str = strdup(str);
    if(!new_str) { ERROR("string copy failed"); }
    return new_str;
}

/*
 * Function that decides if two strings are equal. Returns 1 (true) if they are
 * the same, or 0 (false) if they differ.
 */
bool str_equal(char *str1, char *str2) {
    // If the pointers are to the same string, or they're both null, return true
    if(str1 == str2) { return true; }
    // If only one is null, return false
    else if(!(str1 && str2)) { return false; }
	// If they have different lengths, we can say immediately that they differ
    else if(strlen(str1) != strlen(str2)) { return 0; }
	// If they are she same length, we must use strncmp to compare them. strncmp
	// returns 0 for identical strings, and other ints for different ones, so we
	// negate the result.
	else { return !strncmp(str1, str2, strlen(str1)); }
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

/*
 * Parse an integer from a null-terminated string. Return the integer in an
 * IntRef if parsing was successful, or null if it fails.
 */
IntRef *str_to_int(char *str) {
    int result = 0;
    char *char_ptr = str;
    bool no_errors = true;
    while(*char_ptr != '\0' && no_errors) {
        if(isdigit(*char_ptr)) {
            result = (result * 10) + (*char_ptr - '0');
            char_ptr++;
        }
        else {
            no_errors = false;
        }
    }
    return no_errors ? IntRef_init(result) : NULL;
}

/*
 * Free a string. Required for DEFINE_VECTORABLE(char).
 */
void char_free(char *str) { free(str); }

// For Vectors of strings
DEFINE_VECTORABLE(char)

/*
 * Construct a new IntRef.
 */
IntRef *IntRef_init(int val) {
    IntRef *ir = challoc(sizeof(IntRef));
    IntRef_value(ir) = val;
    return ir;
}

/*
 * Test IntRefs for equality. They are considered if they point to an integer of
 * the same value.
 */
bool IntRef_eq(IntRef *i1, IntRef *i2) {
    return IntRef_value(i1) == IntRef_value(i2);
}

/*
 * Free a IntRef. Required for DEFINE_VECTORABLE(IntRef).
 */
void IntRef_free(IntRef *ri) { free(ri); }

// For Vectors of integers
DEFINE_VECTORABLE(IntRef)
