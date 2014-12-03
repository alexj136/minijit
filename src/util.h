#ifndef util
#define util

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
