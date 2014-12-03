#ifndef util
#define util

#define VECTOR_INITIAL_ARR_SIZE 10

#define DEFINE_VECTORABLE(ty) \
    \
    /* Define a Vector type containing elements of type 'ty'. */ \
    typedef struct ty##Vector ty##Vector; \
    struct ty##Vector { \
        /* number of elements */        int size \
        /* size of underlying array */  int arr_size; \
        /* pointer to array*/           ty **arr; \
    }; \
    \
    /* Initialise a Vector */ \
    ty##Vector **ty##Vector_init() { \
        ty##Vector *vec = challoc(sizeof(ty##Vector)); \
        vec->size = 0; \
        vec->arr_size = VECTOR_INITIAL_ARR_SIZE; \
        vec->arr = challoc(sizeof(ty *) * size); \
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
            vec->arr = \
                    chrealloc(vec->arr, sizeof(ty *) * (vec->arr_size * 2)); \
            vec->arr_size *= 2; \
        } \
        vec->arr[vec->size] = elem; \
        vec->size++; \
    } \


void *challoc(int size);

void *chrealloc(void *ptr, int size);

void put_indent(int num);

typedef enum { false, true } bool;

#endif // util
