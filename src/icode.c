#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"

/*
 * ICodeOperation functions
 */

ICodeOperation *ICodeOperation_init(Opcode opc, int arg1, int arg2) {
    ICodeOperation *op = challoc(sizeof(ICodeOperation));
    op->opc       = opc;
    op->arg1      = arg1;
    op->arg2      = arg2;
    return op;
}

bool ICodeOperation_eq(ICodeOperation *op1, ICodeOperation *op2) {
    // Null checks are essentially to supress the unused parameter warning
    if(!op1) { ERROR("Null Operation"); }
    if(!op2) { ERROR("Null Operation"); }
    ERROR("Equality not defined for Operation");
}

void ICodeOperation_free(ICodeOperation *op) { free(op); }

void ICodeOperation_print(ICodeOperation *op) {

    if(op->opc == MOVE) {
        printf("MOVE ");
        reg_print(op->arg1);
        printf(" ");
        reg_print(op->arg2);
        printf("\n");
    }
    else if(op->opc == LOAD) {
        printf("LOAD ");
        reg_print(op->arg1);
        printf(" ");
        reg_print(op->arg2);
        printf("\n");
    }
    else if(op->opc == STORE) {
        printf("STORE ");
        reg_print(op->arg1);
        printf(" ");
        reg_print(op->arg2);
        printf("\n");
    }
    else if (op->opc == ADD) {
        printf("ADD ");
        reg_print(op->arg1);
        printf(" ");
        reg_print(op->arg2);
        printf("\n");
    }
    else if (op->opc == SUB) {
        printf("SUB ");
        reg_print(op->arg1);
        printf(" ");
        reg_print(op->arg2);
        printf("\n");
    }
    else if (op->opc == LOADIMM) {
        printf("LOADIMM %d ", op->arg1);
        reg_print(op->arg2);
        printf("\n");
    }
    else if (op->opc == JUMP) {
        printf("JUMP L%d\n", op->arg1);
    }
    else if (op->opc == JUMPCOND) {
        printf("JUMPCOND L%d ", op->arg1);
        reg_print(op->arg2);
        printf("\n");
    }
    else if (op->opc == JUMPLINK) {
        printf("JUMPLINK L%d\n", op->arg1);
    }
    else if (op->opc == JUMPADDR) {
        printf("JUMPADDR %d\n", op->arg1);
    }
    else if (op->opc == LABEL) {
        printf("--- L%d ---\n", op->arg1);
    }
    else if (op->opc == HALT) {
        printf("HALT\n");
    }
    else {
        ERROR("Invalid Opcode");
    }
}

void reg_print(int reg) {
    if(reg < 0) {
        printf("Reg %d invalid.\n", reg);
        ERROR("Invalid Register");
    }
    else if(reg == STACK_POINTER) {
        printf("STACK_POINTER");
    }
    else if(reg == FRAME_POINTER) {
        printf("FRAME_POINTER");
    }
    else if(reg == RETURN_ADDRESS) {
        printf("RETURN_ADDRESS");
    }
    else if(reg == PROGRAM_COUNTER) {
        printf("PROGRAM_COUNTER");
    }
    else if(reg == ACCUMULATOR) {
        printf("ACCUMULATOR");
    }
    else if(reg == TEMPORARY) {
        printf("TEMPORARY");
    }
    else {
        printf("R%d", reg - 6);
    }
}

DEFINE_VECTORABLE(ICodeOperation)

void ICodeOperationVector_print(ICodeOperationVector *ops) {
    int idx;
    for(idx = 0; idx < ICodeOperationVector_size(ops); idx++) {
        ICodeOperation_print(ICodeOperationVector_get(ops, idx));
    }
}
