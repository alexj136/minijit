#include <stdio.h>
#include <stdlib.h>
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"
#include "icode_interpreter.h"

ICodeInterpreterState *ICodeInterpreterState_init(ICodeOperationVector *code,
        int stack_size, int next_reg, int next_label) {

    ICodeInterpreterState *state = challoc(sizeof(ICodeInterpreterState));
    state->code       = code;
    state->stack_size = stack_size;
    state->stack      = challoc(sizeof(int) * stack_size);
    state->next_reg   = next_reg;
    state->registers  = challoc(sizeof(int) * next_reg);
    state->next_label = next_label;
    state->labels     = challoc(sizeof(int) * next_label);

    // Populate the labels array. The labels array maps label names to indexes
    // into the ICodeOperationVector.
    int idx;
    for(idx = 0; idx < ICodeOperationVector_size(code); idx++) {
        ICodeOperation *next = ICodeOperationVector_get(code, idx);
        if(next->opc == LABEL) {
            (state->labels)[next->arg1] = idx;
        }
    }

    return state;
}

void prepare_stack(ICodeInterpreterState *state, Prog *prog,
        IntRefVector *args) {

    int num_args     = IntRefVector_size(args);
    int num_vars     = Func_num_vars(Prog_func(prog, 0));
    int non_arg_vars = num_vars - num_args;

    int stack_idx    = num_vars - 1;
    int args_idx     = 0;
    while(stack_idx >= non_arg_vars) {
        (state->stack)[stack_idx] =
                IntRef_value(IntRefVector_get(args, args_idx));
        stack_idx--;
        args_idx++;
    }
    while(stack_idx >= 0) {
        (state->stack)[stack_idx] = 0;
        stack_idx--;
    }
}

void ICodeInterpreterState_step(ICodeInterpreterState *state) {

    int initial_prog_counter_value = (state->registers)[PROGRAM_COUNTER];

    ICodeOperation *current =
            ICodeOperationVector_get(state->code, initial_prog_counter_value);

    Opcode opc = current->opc;
    int arg1   = current->arg1;
    int arg2   = current->arg2;

    if(opc == MOVE) {

        // Do the move
        (state->registers)[arg2] = (state->registers)[arg1];

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == LOAD) {

        // Do the load
        (state->registers)[arg2] = (state->stack)[(state->registers)[arg1]];

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == STORE) {

        // Do the store
        (state->stack)[(state->registers)[arg2]] = (state->registers)[arg1];

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == ADD) {

        // Do the addition
        (state->registers)[arg1] =
                (state->registers)[arg1] + (state->registers)[arg2];

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == SUB) {

        // Do the subtraction
        (state->registers)[arg1] =
                (state->registers)[arg1] - (state->registers)[arg2];

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == LOADIMM) {

        // Do the load immediate
        (state->registers)[arg2] = arg1;

        // Increment the program counter
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == JUMP) {

        // Set the program counter at the target label
        (state->registers)[PROGRAM_COUNTER] = (state->labels)[arg1];
    }
    else if(opc == JUMPCOND) {

        // Based on the condition, optionally set the program counter at the
        // target label
        if(arg2 < 1) {
            (state->registers)[PROGRAM_COUNTER] = (state->labels)[arg1];
        }
    }
    else if(opc == JUMPLINK) {

        // Store the address of the next instruction in the return address
        // register
        (state->registers)[RETURN_ADDRESS] =
                (state->registers)[PROGRAM_COUNTER] + 1;

        // Set the program counter at the target label
        (state->registers)[PROGRAM_COUNTER] = (state->labels)[arg1];
    }
    else if(opc == LABEL) {

        // Labels have no effect other than being jump destinations. Just
        // increment the program counter.
        (state->registers)[PROGRAM_COUNTER]++;
    }
    else if(opc == HALT) {

        // Do nothing - we've halted
    }
    else {
        ERROR("Opcode not recognised");
    }
}

void ICodeInterpreterState_run(ICodeInterpreterState *state) {

    // Until we reach a halt instruction...
    while((ICodeOperationVector_get(state->code,
            (state->registers)[PROGRAM_COUNTER])->opc) != HALT) {

        // Step through the next instruction.
        ICodeInterpreterState_step(state);
    }
}
