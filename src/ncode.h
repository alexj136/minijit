#ifndef ncode
#define ncode

typedef unsigned char byte;

#define ADD_to_x86_64(r1, r2) \
    \
    0x01, reg_pair_code(r1, r2)

#define SUB_to_x86_64(r1, r2) \
    \
    0x29, reg_pair_code(r1, r2)

/*
 * Translate the icode HALT instruction to x86_64.
 */
#define HALT_to_x86_64 \
    \
    /* Some operation to save the accumulator value */ \
    \
    /* movq $60, %rax ; Put $60 exit syscall code in correct register */ \
    0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, \
    \
    /* Do the syscall */ \
    x86_64_syscall

#define x86_64_syscall 0x0F, 0x05

byte reg_pair_code(int r1, int r2) {
    if((r1 > TEMPORARY) || (r1 < STACK_POINTER) ||
            (r2 > TEMPORARY) || (r2 < STACK_POINTER)) {

        ERROR("Invalid register");
    }
    else if((r1 == PROGRAM_COUNTER) || (r2 == PROGRAM_COUNTER)) {
        ERROR("Cannot modify or use program counter register");
    }
    else { return reg_lookup_table[r1][r2]; }
}

byte reg_lookup_table[6][6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESP     EBP     ECX     EIP     EAX     EBX */
    /* SP  ESP */ { 0xE4,   0xEC,   0xCC,   0x00,   0xC4,   0xDC },
    /* FP  EBP */ { 0xE5,   0xED,   0xCD,   0x00,   0xC5,   0xDD },
    /* RA  ECX */ { 0xE1,   0xE9,   0xC9,   0x00,   0xC1,   0xD9 },
    /* PC  EIP */ { 0x00,   0x00,   0x00,   0x00,   0x00,   0x00 },
    /* ACC EAX */ { 0xE0,   0xE8,   0xC8,   0x00,   0xC0,   0xD8 },
    /* TMP EBX */ { 0xE3,   0xEB,   0xCB,   0x00,   0xC3,   0xDB }
};

#endif // ncode
