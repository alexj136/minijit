#ifndef vmcodegen
#define vmcodegen

#define VMCodeBuffer_instruction_size 3
typedef VMCodeBuffer int

VMCodeBuffer *VMCodeGen(ICodeOperationVector *code);

#endif // vmcodegen
