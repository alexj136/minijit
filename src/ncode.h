#ifndef ncode
#define ncode

#define reg_to_x86_64 \
    \

#define ADD_to_x86_64 \
    \

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

#endif // ncode
