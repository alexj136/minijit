#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "syntax.h"
#include "icode.h"
#include "icodegen.h"
#include "icode_interpreter.h"

#define ICodeInterpreterState_STACK_SIZE 100000

ICodeInterpreterState *ICodeInterpreterState_init(ICodeOperationVector *code) {

    int idx;
    int min_label = INT_MAX;
    int max_label = 0;
    int max_reg   = 5;

    // Perform a scan of the icode to determine the maximum and minimum label
    // numbers, so that we need not allocate a huge amount of memory for label
    // numbers not needed. Also determine how much memory we need to allocate
    // for registers.
    for(idx = 0; idx < ICodeOperationVector_size(code); idx++) {
        ICodeOperation *next = ICodeOperationVector_get(code, idx);

        // Check for new maximum register
        if((next->opc == MOVE)
                || (next->opc == LOAD)
                || (next->opc == STORE)
                || (next->opc == ADD)
                || (next->opc == SUB)) {

            if(next->arg1 > max_reg) {
                max_reg = next->arg1;
            }
            if(next->arg2 > max_reg) {
                max_reg = next->arg2;
            }
        }

        // Check for new maximum register (arg1 is not a register in LOADIMM)
        else if(next->opc == LOADIMM) {
            if(next->arg2 > max_reg) {
                max_reg = next->arg2;
            }
        }

        // No registers in a JUMP
        else if(next->opc == JUMP) {
            // Nothing to do here
        }

        else if(next->opc == JUMPCOND) {
        }

        // No explicit registers in a JUMPLINK (implicit RETURN_ADDRESS need not
        // be accounted for since it is less in its index than the minimum
        // size allowed for the registers array)
        else if(next->opc == JUMPLINK) {
            // Nothing to do here
        }

        // arg2 is not a register in JUMPADDR
        else if(next->opc == JUMPADDR) {
            if(next->arg1 > max_reg) {
                max_reg = next->arg1;
            }
        }

        // Check if we have the new minimum/maximum label
        else if(next->opc == LABEL) {
            if(next->arg1 < 0) {
                ERROR("Negative label number in an ICodeOperationVector");
            }
            if(next->arg1 > max_label) {
                max_label = next->arg1;
            }
            if(next->arg1 < min_label) {
                min_label = next->arg1;
            }
        }

        // No registers in a HALT
        else if(next->opc == HALT) {
            // Nothing to do here
        }

        else {
            ERROR("Unrecognised Opcode in ICodeOperation");
        }
    }

    ICodeInterpreterState *state = challoc(sizeof(ICodeInterpreterState));
    state->code      = code;
    state->stack     = challoc(sizeof(int) * ICodeInterpreterState_STACK_SIZE);
    state->registers = challoc(sizeof(int) * (max_reg + 1));
    state->labels    = challoc(sizeof(int) * (max_label - min_label + 1));
    state->min_label = min_label;

    // Populate the labels array. The labels array maps label names to indexes
    // into the ICodeOperationVector.
    for(idx = 0; idx < ICodeOperationVector_size(code); idx++) {
        ICodeOperation *next = ICodeOperationVector_get(code, idx);
        if(next->opc == LABEL) {
            (state->labels)[(next->arg1) - min_label] = idx;
        }
    }

    return state;
}

int ICodeInterpreterState_lookup_label_address(ICodeInterpreterState *state,
        int label_name) {

    return (state->labels)[label_name - (state->min_label)];
}

/*
 * Free an ICodeInterpreterState. Does not free the stored program code.
 */
void ICodeInterpreterState_free(ICodeInterpreterState *state) {
    free(state->stack);
    free(state->registers);
    free(state->labels);
    free(state);
    return;
}

/*
 * Get the result (i.e. final accumulator value) of a halted interpreter run.
 * It is an error to call this on a state that has not halted.
 */
int ICodeInterpreterState_result(ICodeInterpreterState *state) {

    if((ICodeOperationVector_get(state->code,
            (state->registers)[PROGRAM_COUNTER])->opc) == HALT) {

        return (state->registers)[ACCUMULATOR];
    }
    else {
        ERROR("Tried to retreive the result of a non-halted execution");
    }
}

/*
 * Prepare an ICodeInterpreterState for exection by putting the appropriate
 * values on the stack to call the program's main function.
 */
void prepare_state(ICodeInterpreterState *state, Prog *prog,
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

    (state->registers)[STACK_POINTER]   = num_vars;
    (state->registers)[FRAME_POINTER]   = 0;
    (state->registers)[RETURN_ADDRESS]  = 0;
    (state->registers)[PROGRAM_COUNTER] = 0;
}

/*
 * Perform a single execution step
 */
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
        (state->registers)[PROGRAM_COUNTER] =
                ICodeInterpreterState_lookup_label_address(state, arg1);
    }
    else if(opc == JUMPCOND) {

        // Based on the condition, optionally set the program counter at the
        // target label, or just advance to the next instruction.
        if(arg2 < 1) {
            (state->registers)[PROGRAM_COUNTER] =
                ICodeInterpreterState_lookup_label_address(state, arg1);
        }
        else {
            (state->registers)[PROGRAM_COUNTER]++;
        }
    }
    else if(opc == JUMPLINK) {

        // Store the address of the next instruction in the return address
        // register
        (state->registers)[RETURN_ADDRESS] =
                (state->registers)[PROGRAM_COUNTER] + 1;

        // Set the program counter at the target label
        (state->registers)[PROGRAM_COUNTER] =
                ICodeInterpreterState_lookup_label_address(state, arg1);
    }
    else if(opc == JUMPADDR) {

        // Set the program counter at the address in the register
        (state->registers)[PROGRAM_COUNTER] = (state->registers)[arg1];
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

/*
 * Run an ICodeInterpreterState until the program halts (although it may not
 * ever halt).
 */
void ICodeInterpreterState_run(ICodeInterpreterState *state) {

    // Until we reach a halt instruction...
    while((ICodeOperationVector_get(state->code,
            (state->registers)[PROGRAM_COUNTER])->opc) != HALT) {

        // Step through the next instruction.
        ICodeInterpreterState_step(state);
    }
}
