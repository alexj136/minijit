#ifndef vmcodegen
#define vmcodegen

#define VMCode_instruction_size (sizeof(ICodeOperation))

int *VMCodeGen(ICodeOperationVector *code);
void VMCode_print(int *vmcode, int num_instructions);

#endif // vmcodegen
