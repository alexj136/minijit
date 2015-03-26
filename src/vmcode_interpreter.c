#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"
#include "icode_interpreter.h"
#include "vmcodegen.h"
#include "vmcode_interpreter.h"

int VMCode_execute(ICodeOperationVector *code, IntRefVector *initial_stack) {
    VMCodeInterpreterState* vmstate =
            VMCodeInterpreterState_init(code, initial_stack);
    VMCodeInterpreterState_run(vmstate);
    int result = (vmstate->registers)[ACCUMULATOR];
    VMCodeInterpreterState_free(vmstate);
    return result;
}

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

void VMCodeInterpreterState_free(VMCodeInterpreterState *state) {
    free(state->code);
    free(state->stack);
    free(state->registers);
    free(state->labels);
    free(state);
}

#define OPC  code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 0]
#define ARG1 code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 1]
#define ARG2 code[(registers[PROGRAM_COUNTER] * VMCode_instruction_size) + 2]
#define VMCodeInterpreterState_lookup_label_address(label_name) \
        (labels[label_name - min_label])

void VMCodeInterpreterState_run(VMCodeInterpreterState *state) {

    // Copy the pointers in the VMCodeInterpreterState to minimise indirection
    int *code      = state->code;
    int *stack     = state->stack;
    int *registers = state->registers;
    int *labels    = state->labels;
    int min_label  = state->min_label;

    while(true) {
        if(OPC == MOVE) {
            registers[ARG2] = registers[ARG1];
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == LOAD) {
            registers[ARG2] = stack[registers[ARG1]];
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == STORE) {
            stack[registers[ARG2]] = registers[ARG1];
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == ADD) {
            registers[ARG1] = registers[ARG1] + registers[ARG2];
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == SUB) {
            registers[ARG1] = registers[ARG1] - registers[ARG2];
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == LOADIMM) {
            registers[ARG2] = ARG1;
            registers[PROGRAM_COUNTER]++;
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
                registers[PROGRAM_COUNTER]++;
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
            registers[PROGRAM_COUNTER]++;
        }
        else if(OPC == HALT) {
            break;
        }
        else {
            ERROR("Unrecognised Opcode");
        }
    }
}
