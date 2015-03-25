#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"
#include "icode_interpreter.h"
#include "vmcodegen.h"
#include "vmcode_interpreter.h"

VMCodeInterpreterState *VMCodeInterpreterState_init(
        ICodeOperationVector *code, IntRefVector *initial_stack) {

    ICodeInterpreterState  *icode_state  =
            ICodeInterpreterState_init(code, initial_stack);

    VMCodeInterpreterState *vmcode_state =
            challoc(sizeof(VMCodeInterpreterState));

    vmcode_state->code      = VMCodeGen(code);
    vmcode_state->stack     = icode_state->stack;
    vmcode_state->registers = icode_state->registers;
    vmcode_state->labels    = icode_state->labels;
    vmcode_state->min_label = icode_state->min_label;
    free(icode_state);
    return vmcode_state;
}

#define OPC  code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 0]
#define ARG1 code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 1]
#define ARG2 code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 2]
#define ADVANCE_PC registers[PROGRAM_COUNTER] += VMCode_instruction_size
#define VMCodeInterpreterState_lookup_label_address(label_name) \
    labels[(label_name - min_label) * VMCode_instruction_size]

void VMCode_run(VMCodeInterpreterState *state) {

    // Copy the pointers in the VMCodeInterpreterState to minimise indirection
    int *code      = state->code;
    int *stack     = state->stack;
    int *registers = state->registers;
    int *labels    = state->labels;
    int min_label  = state->min_label;

    while(true) {
        if(OPC == MOVE) {
            registers[ARG2] = registers[ARG1];
            ADVANCE_PC;
        }
        else if(OPC == LOAD) {
            registers[ARG2] = stack[registers[ARG1]];
            ADVANCE_PC;
        }
        else if(OPC == STORE) {
            stack[registers[ARG2]] = registers[ARG1];
            ADVANCE_PC;
        }
        else if(OPC == ADD) {
            registers[ARG1] = registers[ARG1] + registers[ARG2];
            ADVANCE_PC;
        }
        else if(OPC == SUB) {
            registers[ARG1] = registers[ARG1] - registers[ARG2];
            ADVANCE_PC;
        }
        else if(OPC == LOADIMM) {
            registers[ARG2] = ARG1;
            ADVANCE_PC;
        }
        else if(OPC == JUMP) {
            registers[PROGRAM_COUNTER] =
                    VMCodeInterpreterState_lookup_label_address(ARG1);
        }
        else if(OPC == JUMPCOND) {
            if(registers[ARG2] < 1) {
                registers[PROGRAM_COUNTER] =
                        VMCodeInterpreterState_lookup_label_address(ARG1);
            }
            else {
                ADVANCE_PC;
            }
        }
        else if(OPC == JUMPLINK) {
            registers[RETURN_ADDRESS] = registers[PROGRAM_COUNTER] + 1;
            registers[PROGRAM_COUNTER] =
                    VMCodeInterpreterState_lookup_label_address(ARG1);
        }
        else if(OPC == JUMPADDR) {
            registers[PROGRAM_COUNTER] = registers[ARG1];
        }
        else if(OPC == LABEL) {
            ADVANCE_PC;
        }
        else if(OPC == HALT) {
            break;
        }
        else {
            ERROR("Unrecognised Opcode");
        }
    }
}
