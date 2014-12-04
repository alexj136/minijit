#ifndef util
#define util

/*
 * VECTOR MACROS
 * The following macros define 'generic' Vector functions, over struct types,
 * declared in header files, in the following way:
 *     typedef struct MyStruct MyStruct; struct Mystruct { ... };
 * The Vectors are able to store pointers to structs of types declared in this
 * way only. The type must also have an associated function of the form:
 *     MyStruct_free(MyStruct *ms) { ... }
 * that frees structs of this type, and is forward-declared in a header file.
 * To use these macros, include e.g.
 *     FORWARD_DECLARE_VECTORABLE(MyStruct)
 * in the header file where the type is declared, and
 *     DEFINE_VECTORABLE(MyStruct)
 * in the file where the freeing function is declared.
 *
 * The defined Vector type supports log(n) time insertion and constant time
 * access. Deletion is not yet supported.
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
    ty *ty##Vector_get(ty##Vector *vec, int index); \
    void ty##Vector_free(ty##Vector *vec); \
    void ty##Vector_free_elems(ty##Vector *vec);

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
        return vec->size; \
    } \
    \
    /* Append a new element of type 'ty' to a Vector of elements of that */ \
    /* type. */ \
    void ty##Vector_append(ty##Vector *vec, ty *elem) { \
        if(vec->size >= vec->arr_size) { \
            vec->arr = chrealloc(vec->arr, sizeof(ty *) * 2 * \
                    (vec->arr_size)); \
            vec->arr_size *= 2; \
        } \
        vec->arr[vec->size] = elem; \
        vec->size++; \
    } \
    \
    /* Retreive an element from a Vector at the given index */ \
    ty *ty##Vector_get(ty##Vector *vec, int index) { \
        if(index >= vec->size) { \
            puts("Error: ty##Vector index out of bounds."); \
            exit(EXIT_FAILURE); \
        } \
        else if(index < 0) { \
            puts("Error: Negative ty##Vector index."); \
            exit(EXIT_FAILURE); \
        } \
        else { return vec->arr[index]; } \
    } \
    \
    /* Free a Vector, but not the elements. */ \
    void ty##Vector_free(ty##Vector *vec) { \
        free(vec->arr); \
        free(vec); \
    } \
    /* Free a Vector and its elements. */ \
    void ty##Vector_free_elems(ty##Vector *vec) { \
        int idx; \
        for(idx = 0; idx < vec->size; idx++) { \
            ty##_free(vec->arr[idx]); \
        } \
        free(vec->arr); \
        free(vec); \
    } \

void *challoc(int size);

void *chrealloc(void *ptr, int size);

void put_indent(int num);

typedef enum { false, true } bool;

#endif // util
