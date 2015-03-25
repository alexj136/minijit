#ifndef icode
#include "icode.h"
#endif // icode

#ifndef vmcodegen
#define vmcodegen

#define VMCode_instruction_size sizeof(ICodeOperation)

int *VMCodeGen(ICodeOperationVector *code);

#endif // vmcodegen
