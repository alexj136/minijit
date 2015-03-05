#ifndef util
#include "util.h"
#endif // util

#ifndef syntax
#include "syntax.h"
#endif // syntax

#ifndef typecheck
#define typecheck

/*
 * Types
 */

typedef enum { typeVoid, typeInt, typeFunc } TypeForm;

typedef struct Type Type;
struct Type {
    TypeForm form;
    struct Type *innerType;
    struct TypeVector *innerTypes;
};

FORWARD_DECLARE_VECTORABLE(Type)

// Constructors
Type *FuncType_init(TypeVector *argTypes, Type *returnType);
Type *IntType_init();
Type *VoidType_init();

// Methods
Type *Type_copy(Type *old);
bool Type_eq(Type *t1, Type *t2);
void Type_free(Type *type);

/*
 * TypeErrors
 */

typedef struct TypeError TypeError;
struct TypeError {
    int name;
    struct Type *found;
    struct Type *expected;
    int line_no;
    int char_no;
};

FORWARD_DECLARE_VECTORABLE(TypeError)

// Constructor
TypeError *TypeError_init(int name, Type *found, Type *expected, int line_no,
        int char_no);

// Methods
bool TypeError_eq(TypeError *error1, TypeError *error2);
void TypeError_free(TypeError *error);

/*
 * TypeCheckResults
 */

typedef struct TypeCheckResult TypeCheckResult;
struct TypeCheckResult {
    bool success;
    Type *type;
    TypeErrorVector *errors;
};

// Constructors
TypeCheckResult *TypeCheckSuccess_init(Type *type);
TypeCheckResult *TypeCheckFail_init(TypeErrorVector *errors);

// Methods
bool TypeCheckResult_isTypeForm(TypeCheckResult *result, TypeForm form);
void TypeCheckResult_free(TypeCheckResult *result);

#endif // typecheck
