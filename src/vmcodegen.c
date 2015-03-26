#include <stdio.h>
#include <string.h>
#include "util.h"
#include "icode.h"
#include "vmcodegen.h"

/*
 * VMCode is simply a continuous block of memory containing the instruction
 * information contained in ICodeOperation structs. This gives faster execution
 * since access locality is improved. The interpreter loop for VMCode is tighter
 * and does fewer checks and so is also faster than the ICode interpreter.
 */
int *VMCodeGen(ICodeOperationVector *code) {

    int *out = challoc(sizeof(int) * VMCode_instruction_size *
            ICodeOperationVector_size(code));

    int *out_ptr = out;

    int icode_idx;
    for(icode_idx = 0; icode_idx < ICodeOperationVector_size(code);
            icode_idx++) {

        ICodeOperation *op = ICodeOperationVector_get(code, icode_idx);

        memcpy(out_ptr, op, sizeof(ICodeOperation));

        out_ptr += sizeof(ICodeOperation);
    }

    return out;
}

void VMCode_print(int *vmcode, int num_instructions) {
    int idx = 0;
    while(idx < num_instructions) {
        ICodeOperation_print(
                (ICodeOperation *) (vmcode + (idx * VMCode_instruction_size)));
        idx++;
    }
}
