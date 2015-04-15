#ifndef ncode
#define ncode

typedef byte unsigned char

#define EAX_EAX 0xc0
#define EAX_EBX 0xc3
#define EAX_ECX 0xc1
#define EAX_EDX 0xc2

#define EBX_EAX 0xd8
#define EBX_EBX 0xdb
#define EBX_ECX 0xd9
#define EBX_EDX 0xda

#define ECX_EAX 0xc8
#define ECX_EBX 0xcb
#define ECX_ECX 0xc9
#define ECX_EDX 0xca

#define EDX_EAX 0xd0
#define EDX_EBX 0xd3
#define EDX_ECX 0xd1
#define EDX_EDX 0xd2

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

byte reg_pair_code(byte r1, byte r2) {

}

#endif // ncode
