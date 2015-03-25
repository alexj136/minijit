#include <stdio.h>
#include <string.h>
#include "util.h"
#include "syntax.h"
#include "icode.h"
#include "vmcodegen.h"

int *VMCodeGen(ICodeOperationVector *code) {

    int *out = challoc(sizeof(int) * VMCodeBuffer_instruction_size *
            ICodeOperationVector_size(code));

    int idx;
    for(idx = 0; idx < ICodeOperationVector_size(code); idx++) {

        ICodeOperation *op = ICodeOperationVector_get(code, idx);

        memcpy(out + (idx * VMCodeBuffer_instruction_size), op,
                sizeof(ICodeOperation));
    }

    return out;
}
