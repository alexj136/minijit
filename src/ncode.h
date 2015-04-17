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

byte reg_pair_code(int r1, int r2);

#endif // ncode
