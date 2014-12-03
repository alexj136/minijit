#ifndef util
#define util

#define RefVectorable(ty) \
    typedef struct ty##Vector ty##Vector; \
    struct ty##Vector { \
        int length; \
        ty **arr; \
    };

void *challoc(int size);

void *chrealloc(void *ptr, int size);

void put_indent(int num);

typedef enum { false, true } bool;

#endif // util
