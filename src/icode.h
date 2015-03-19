#ifndef util
#include "util.h"
#endif // util

#ifndef icode
#define icode

/*
 * Register names
 */

#define STACK_POINTER   (0)
#define FRAME_POINTER   (1)
#define RETURN_ADDRESS  (2)
#define PROGRAM_COUNTER (3)
#define ACCUMULATOR     (4)
#define TEMPORARY       (5)
#define REGISTER(num)   (num + 6)

/*
 * Operation structs - intermediate level representation
 */

typedef enum
    { MOVE      // Move the contents of register arg1 to register arg2
    , LOAD      // Load the value at address in register arg1 into register arg2
    , STORE     // Store the value in register arg1 at address in register arg2
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

typedef struct ICodeOperation ICodeOperation;
struct ICodeOperation {
    Opcode opc;
    int arg1;
    int arg2;
};

ICodeOperation *ICodeOperation_init(Opcode opc, int arg1, int arg2);
bool ICodeOperation_eq(ICodeOperation *op1, ICodeOperation *op2);
void ICodeOperation_free(ICodeOperation *op);
void ICodeOperation_print(ICodeOperation *op);
void reg_print(int reg);

FORWARD_DECLARE_VECTORABLE(ICodeOperation)

void ICodeOperationVector_print(ICodeOperationVector *ops);

#endif // icode
