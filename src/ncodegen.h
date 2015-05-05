#ifndef ncodegen
#define ncodegen

typedef struct NCodeLinkable NCodeLinkable;
struct NCodeLinkable {
    byte *ncode;
    int ncode_size;
    struct IntRefVector *address_locations;
};

typedef struct NCodeLinked NCodeLinked;
struct NCodeLinkable {
    byte *ncode;
    int ncode_size;
};

#endif // ncodegen
