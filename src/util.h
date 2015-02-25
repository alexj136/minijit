#ifndef util
#define util

// Booleans
typedef enum { false, true } bool;

/*
 * A struct to store only an integer. Makes integer Vectors definable with
 * Vector macros (although there is a speed penalty here due to unnecessary
 * indirection).
 */
typedef struct IntRef IntRef;
struct IntRef {
    int value;
};

/*
 * VECTOR MACROS
 * The following macros define 'generic' Vector functions, over struct types,
 * declared in header files, in the following way:
 *     typedef struct MyStruct MyStruct;
 *     struct MyStruct { ... };
 * The Vectors are able to store pointers to structs of types declared in this
 * way only. The type must also have associated functions of the form:
 *
 *     // Frees the struct
 *     MyStruct_free(MyStruct *ms) { ... }
 *
 *     // Determines if two struct instances are equal
 *     MyStruct_eq(MyStruct *ms1, MyStruct *ms2) { ... }
 *
 * that frees structs of this type, and is forward-declared in a header file.
 * To use these macros, include e.g.
 *     FORWARD_DECLARE_VECTORABLE(MyStruct)
 * in the header file where the type is declared, and
 *     DEFINE_VECTORABLE(MyStruct)
 * in the file where the freeing function is declared.
 *
 * The defined Vector type supports log(n) time appending, linear time insertion
 * for arbitrary insertion indices, constant time access, and linear time
 * removal.
 */

#define VECTOR_INITIAL_ARR_SIZE 10

#define FORWARD_DECLARE_VECTORABLE(ty) \
    \
    /* Define a Vector type containing elements of type 'ty'. */ \
    typedef struct ty##Vector ty##Vector; \
    struct ty##Vector { \
        /* number of elements */        int size; \
        /* size of underlying array */  int arr_size; \
        /* pointer to array*/           ty **arr; \
    }; \
    \
    ty##Vector *ty##Vector_init(); \
    int ty##Vector_size(ty##Vector *vec); \
    void ty##Vector_append(ty##Vector *vec, ty *elem); \
    void ty##Vector_append_all(ty##Vector *vec, ty##Vector *new_elems); \
    void ty##Vector_insert(ty##Vector *vec, int index, ty *elem); \
    ty *ty##Vector_get(ty##Vector *vec, int index); \
    ty##Vector *ty##Vector_concat(ty##Vector *v1, ty##Vector *v2); \
    void ty##Vector_free(ty##Vector *vec); \
    void ty##Vector_free_elems(ty##Vector *vec); \
    ty *ty##Vector_remove(ty##Vector *vec, int index);\
    bool ty##Vector_eq(ty##Vector *v1, ty##Vector *v2);

#define DEFINE_VECTORABLE(ty) \
    \
    /* Initialise a Vector */ \
    ty##Vector *ty##Vector_init() { \
        ty##Vector *vec = challoc(sizeof(ty##Vector)); \
        vec->size = 0; \
        vec->arr_size = VECTOR_INITIAL_ARR_SIZE; \
        vec->arr = challoc(sizeof(ty *) * VECTOR_INITIAL_ARR_SIZE); \
        return vec; \
    } \
    \
    /* Determine the size (number of elements) of a Vector */ \
    int ty##Vector_size(ty##Vector *vec) { \
        if(!vec) { ERROR("Null Vector"); } \
        return vec->size; \
    } \
    \
    /* Append a new element of type 'ty' to a Vector of elements of that */ \
    /* type. This is a constant time operation. */ \
    void ty##Vector_append(ty##Vector *vec, ty *elem) { \
        if(!vec) { ERROR("Null Vector"); } \
        if(vec->size >= vec->arr_size) { \
            vec->arr = chrealloc(vec->arr, sizeof(ty *) * 2 * \
                    (vec->arr_size)); \
            vec->arr_size *= 2; \
        } \
        vec->arr[vec->size] = elem; \
        vec->size++; \
    } \
    /* Append all elements of a Vector to another Vector */ \
    void ty##Vector_append_all(ty##Vector *vec, ty##Vector *new_elems) { \
        int idx; \
        for(idx = 0; idx < ty##Vector_size(new_elems); idx++) { \
            ty##Vector_append(vec, ty##Vector_get(new_elems, idx)); \
        } \
    } \
    \
    /* Tail-recursively insert an element at a given index into a Vector */ \
    /* in linear time. It is an error to insert an element at an index */ \
    /* greater than the size of the Vector. Indexes equal to the size of */ \
    /* the Vector are permitted - this is equivalent to an append */ \
    /* operation. */ \
    void ty##Vector_insert(ty##Vector *vec, int index, ty *elem) { \
        if(!vec) { ERROR("Null Vector"); } \
        if(index < 0) { ERROR("Negative index."); } \
        else if(index > ty##Vector_size(vec)) { \
            ERROR("Index out of bounds."); \
        } \
        /* If the given index is equal to the length, just do an append. */ \
        else if(index == vec->size) { ty##Vector_append(vec, elem); } \
        else { \
            /* Make a copy of the old element already at the given index. */ \
            ty *old_elem = vec->arr[index]; \
            \
            /* Overwrite with the new element at the given index. */ \
            vec->arr[index] = elem; \
            \
            /* Insert the old element at the index above the given index. */ \
            ty##Vector_insert(vec, index + 1, old_elem); \
        } \
    } \
    \
    /* Retrieve an element from a Vector at the given index. It is an error */ \
    /* to retrieve an element at an index greater than or equal to the */ \
    /* Vector's size. This is a constant time operation. */ \
    ty *ty##Vector_get(ty##Vector *vec, int index) { \
        if(!vec) { ERROR("Null Vector"); } \
        if(index >= ty##Vector_size(vec)) { ERROR("Index out of bounds."); } \
        else if(index < 0) { ERROR("Negative index."); } \
        else { return vec->arr[index]; } \
    } \
    \
    /* Obtain a Vector that is the concatenation of two Vectors. Elements */ \
    /* are not copied, only the references, so be careful when freeing */ \
    /* elements. */ \
    ty##Vector *ty##Vector_concat(ty##Vector *v1, ty##Vector *v2) { \
        ty##Vector *res = ty##Vector_init(); \
        int idx; \
        for(idx = 0; idx < ty##Vector_size(v1); idx++) { \
            ty##Vector_append(res, ty##Vector_get(v1, idx)); \
        } \
        for(idx = 0; idx < ty##Vector_size(v2); idx++) { \
            ty##Vector_append(res, ty##Vector_get(v2, idx)); \
        } \
        return res; \
    } \
    /* Free a Vector, but not the elements. */ \
    void ty##Vector_free(ty##Vector *vec) { \
        if(!vec) { return; } /* Null check */ \
        free(vec->arr); \
        free(vec); \
    } \
    /* Free a Vector and its elements. */ \
    void ty##Vector_free_elems(ty##Vector *vec) { \
        if(!vec) { return; } /* Null check */ \
        int idx; \
        for(idx = 0; idx < vec->size; idx++) { \
            ty##_free(vec->arr[idx]); \
        } \
        free(vec->arr); \
        free(vec); \
    } \
    \
    /* Shift elements backwards within a Vector - used by remove functions. */ \
    /* Expects that the length HAS NOT YET BEEN DECREMENTED by the calling */ \
    /* function, although the calling function is responsible for doing so */ \
    /* after calling this function. */ \
    void ty##Vector_shift(ty##Vector *vec, int index) { \
        if(!vec) { ERROR("Null Vector"); } \
        if(index < 0) { ERROR("Negative index."); } \
        if(index >= ty##Vector_size(vec)) { ERROR("Index out of bounds."); } \
        else if(index == ty##Vector_size(vec) - 1) { \
            vec->arr[index] = NULL; \
        } \
        else { \
            vec->arr[index] = ty##Vector_get(vec, index + 1); \
            ty##Vector_shift(vec, index + 1); \
        } \
    } \
    \
    /* Remove an element from a Vector, returning a pointer to the removed */ \
    /* element for further usage or freeing. */ \
    ty *ty##Vector_remove(ty##Vector *vec, int index) { \
        if(!vec) { ERROR("Null Vector"); } \
        if(index < 0) { ERROR("Negative index."); } \
        if(index >= ty##Vector_size(vec)) { ERROR("Index out of bounds."); } \
        ty *elem = ty##Vector_get(vec, index); \
        ty##Vector_shift(vec, index); \
        vec->size--; \
        return elem; \
    } \
    \
    /* Determine if two vectors are equal. They are deemed equal if they */ \
    /* contain elements that are equal according to the type's definition */ \
    /* of equality, and those elements appear in the same sequence. */ \
    bool ty##Vector_eq(ty##Vector *v1, ty##Vector *v2) { \
        if((!v1) || (!v2)) { return v1 == v2; } /* null check */ \
        bool same = ty##Vector_size(v1) == ty##Vector_size(v2); \
        int idx = 0; \
        while(same && (idx < ty##Vector_size(v1))) { \
            if(!ty##_eq(ty##Vector_get(v1, idx), ty##Vector_get(v2, idx))) { \
                same = false; \
            } \
            idx++; \
        } \
        return same; \
    } \

/*
 * Utility functions
 */

void *challoc(int size);

void *chrealloc(void *ptr, int size);

char *chstrdup(char *str);

void put_indent(int num);

bool str_equal(char *str1, char *str2);

IntRef *str_to_int(char *str);

// For Vectors of strings
#define char_eq str_equal
FORWARD_DECLARE_VECTORABLE(char)

// For Vectors of integers
FORWARD_DECLARE_VECTORABLE(IntRef)
#define IntRef_value(ri) ri->value
IntRef *IntRef_init(int val);
bool IntRef_eq(IntRef *i1, IntRef *i2);
void IntRef_free(IntRef *ri);

// Error macro
#define ERROR(msg) \
    do { \
        printf("ERROR - %s:%d (%s):\n", __FILE__, __LINE__, __func__); \
        printf("    %s\n", msg); \
        exit(EXIT_FAILURE); \
    } while(0)

// Not yet implemented error macro
#define NOT_IMPLEMENTED ERROR("Not yet implemented.")

#endif // util
