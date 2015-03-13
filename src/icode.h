#ifndef util
#include "util.h"
#endif // util

#ifndef icode
#define icode

/*
 * Register names
 */

#define STACK_POINTER   (-4)
#define FRAME_POINTER   (-3)
#define RETURN_ADDRESS  (-2)
#define PROGRAM_COUNTER (-1)
#define REGISTER(num)   (num)
#define ACCUMULATOR     (0)
#define TEMPORARY       (1)

/*
 * Operation structs - intermediate level representation
 */

typedef enum
    { MOVE      // Move the contents of register arg1 to register arg2
    , LOAD      // Load the value at address arg1 into register arg2
    , STORE     // Store the value in register arg1 at address arg2
    , ADD       // Add the contents of register arg1 to the contents of register
                // arg2, leaving the result in register arg1
    , SUB       // Subtract the contents of register arg2 from the contents of
                // register arg1, leaving the result in register arg1
    , LOADIMM   // Load the value arg1 into register arg2
    , JUMP      // Jump to label arg1
    , JUMPCOND  // Jump to label arg1 if the value of register arg2 is less
                // than 1
    , JUMPLINK  // Store the address of the next instruction in RETURN_ADDRESS
                // and jump to label arg1
    , LABEL     // Define the label arg1
    , HALT      // Halt execution and return to caller
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

#endif // icode
