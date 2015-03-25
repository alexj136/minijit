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

    int idx;
    for(idx = 0; idx < ICodeOperationVector_size(code); idx++) {

        ICodeOperation *op = ICodeOperationVector_get(code, idx);

        memcpy(out + (idx * VMCode_instruction_size), op,
                sizeof(ICodeOperation));
    }

    return out;
}
