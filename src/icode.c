#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"

/*
 * Operation functions
 */

Operation *Operation_init(Opcode opc, int arg1, int arg2) {
    Operation *op = challoc(sizeof(Operation));
    op->opc       = opc;
    op->arg1      = arg1;
    op->arg2      = arg2;
    return op;
}

bool Operation_eq(Operation *op1, Operation *op2) {
    // Null checks are essentially to supress the unused parameter warning
    if(!op1) { ERROR("Null Operation"); }
    if(!op2) { ERROR("Null Operation"); }
    ERROR("Equality not defined for Operation");
}

void Operation_free(Operation *op) { free(op); }

void Operation_print(Operation *op) {
    switch(op->opc) {
        case MOVE     : printf("MOVE ");
                        reg_print(op->arg1);
                        printf(" ");
                        reg_print(op->arg2);
                        printf("\n");

        case LOAD     : printf("LOAD ");
                        reg_print(op->arg1);
                        printf(" ");
                        reg_print(op->arg2);
                        printf("\n");

        case STORE    : printf("STORE ");
                        reg_print(op->arg1);
                        printf(" ");
                        reg_print(op->arg2);
                        printf("\n");

        case ADD      : printf("ADD ");
                        reg_print(op->arg1);
                        printf(" ");
                        reg_print(op->arg2);
                        printf("\n");

        case SUB      : printf("SUB ");
                        reg_print(op->arg1);
                        printf(" ");
                        reg_print(op->arg2);
                        printf("\n");

        case LOADIMM  : printf("LOADIMM %d\n", op->arg1);

        case JUMP     : printf("JUMP L%d\n", op->arg1);

        case JUMPCOND : printf("JUMPCOND L%d ", op->arg1);
                        reg_print(op->arg2);
                        printf("\n");

        case JUMPLINK : printf("JUMPLINK L%d\n", op->arg1);

        case LABEL    : printf("--- L%d ---", op->arg1);

        case HALT     : printf("HALT\n");

        default       : ERROR("Invalid Opcode");
    }
}

void reg_print(int reg) {
         if(reg == STACK_POINTER  ) { printf("STACK_POINTER")   ; }
    else if(reg == FRAME_POINTER  ) { printf("FRAME_POINTER")   ; }
    else if(reg == RETURN_ADDRESS ) { printf("RETURN_ADDRESS")  ; }
    else if(reg == PROGRAM_COUNTER) { printf("PROGRAM_COUNTER") ; }
    else if(reg == ACCUMULATOR    ) { printf("ACCUMULATOR")     ; }
    else if(reg == TEMPORARY      ) { printf("TEMPORARY")       ; }
    else if(reg <  -4             ) { ERROR("Invalid Register") ; }
    else                            { printf("R%d", reg)        ; }
}

DEFINE_VECTORABLE(Operation)

void OperationVector_print(OperationVector *ops) {
    int idx;
    for(idx = 0; idx < OperationVector_size(ops); idx++) {
        Operation_print(OperationVector_get(ops, idx));
    }
}
