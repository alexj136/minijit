#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"
#include "ncode.h"

byte dual_reg_lookup_table[6][6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESP     EBP     ECX     EIP     EAX     EBX */
    /* SP  ESP */ { 0xE4,   0xEC,   0xCC,   0x00,   0xC4,   0xDC },
    /* FP  EBP */ { 0xE5,   0xED,   0xCD,   0x00,   0xC5,   0xDD },
    /* RA  ECX */ { 0xE1,   0xE9,   0xC9,   0x00,   0xC1,   0xD9 },
    /* PC  EIP */ { 0x00,   0x00,   0x00,   0x00,   0x00,   0x00 },
    /* ACC EAX */ { 0xE0,   0xE8,   0xC8,   0x00,   0xC0,   0xD8 },
    /* TMP EBX */ { 0xE3,   0xEB,   0xCB,   0x00,   0xC3,   0xDB }
};

byte single_reg_lookup_table[6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESP     EBP     ECX     EIP     EAX     EBX */
                    0xBC,   0xBD,   0xB9,   0x00,   0xB8,   0xBB
};

byte dual_reg_to_x86_64(int r1, int r2) {
    if((r1 > TEMPORARY) || (r1 < STACK_POINTER) ||
            (r2 > TEMPORARY) || (r2 < STACK_POINTER)) {

        ERROR("Invalid register");
    }
    else if((r1 == PROGRAM_COUNTER) || (r2 == PROGRAM_COUNTER)) {
        ERROR("Cannot modify or use program counter register");
    }
    else { return dual_reg_lookup_table[r1][r2]; }
}

byte single_reg_to_x86_64(int r) {
    if((r > TEMPORARY) || (r < STACK_POINTER)) {
        ERROR("Invalid register");
    }
    else if(r == PROGRAM_COUNTER) {
        ERROR("Cannot modify or use program counter register");
    }
    else { return single_reg_lookup_table[r]; }
}
