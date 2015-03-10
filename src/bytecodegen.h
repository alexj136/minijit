#ifndef util
#include "util.h"
#endif // util

#ifndef bytecodegen
#define bytecodegen

/*
 * Register names
 */

#define STACK_POINTER        (-4)
#define FRAME_POINTER        (-3)
#define RETURN_ADDRESS       (-2)
#define PROGRAM_COUNTER      (-1)
#define GENERAL_PURPOSE(num) (num)

/*
 * Operation structs - intermediate level representation
 */

typedef enum
    { LOAD
    , STORE
    , ADD
    , SUB
    , LOADIMM
    , JUMP
    , JUMPIF0
    , JUMPLINK
    , LABEL
    , HALT
    } Opcode;

typedef struct Operation Operation;
struct Operation {
    Opcode opc;
    int arg1;
    int arg2;
};

Operation *Operation_init(Opcode opc, int arg1, int arg2);
bool Operation_eq(Operation *op1, Operation *op2);
void Operation_free(Operation *op);

FORWARD_DECLARE_VECTORABLE(Operation)

#endif // bytecodegen
