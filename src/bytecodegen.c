#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "bytecodegen.h"

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

DEFINE_VECTORABLE(Operation)
