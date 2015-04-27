#ifndef ncode
#define ncode

typedef unsigned char byte;

#define byte0_32(n) ((byte) ((n & 0x000000FF) >>  0))
#define byte1_32(n) ((byte) ((n & 0x0000FF00) >>  8))
#define byte2_32(n) ((byte) ((n & 0x00FF0000) >> 16))
#define byte3_32(n) ((byte) ((n & 0xFF000000) >> 24))

#define byte0_64(n) ((byte) ((n & 0x00000000000000FF) >>  0))
#define byte1_64(n) ((byte) ((n & 0x000000000000FF00) >>  8))
#define byte2_64(n) ((byte) ((n & 0x0000000000FF0000) >> 16))
#define byte3_64(n) ((byte) ((n & 0x00000000FF000000) >> 24))
#define byte4_64(n) ((byte) ((n & 0x000000FF00000000) >> 32))
#define byte5_64(n) ((byte) ((n & 0x0000FF0000000000) >> 40))
#define byte6_64(n) ((byte) ((n & 0x00FF000000000000) >> 48))
#define byte7_64(n) ((byte) ((n & 0xFF00000000000000) >> 56))

#define LOADIMM_to_x86_64(n, r) \
    \
    /* mov $n, %r */ \
    LOADIMM_reg_to_x86_64(r), byte0_32(n), byte1_32(n), byte2_32(n), byte3_32(n)

#define MOVE_to_x86_64(r1, r2) \
    \
    /* mov %r1, %r2 */ \
    0x89, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define ADD_to_x86_64(r1, r2) \
    \
    /* add %r1, %r2 */ \
    0x01, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define SUB_to_x86_64(r1, r2) \
    \
    /* sub %r1, %r2 */ \
    0x29, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define LOAD_to_x86_64(r1, r2) \
    \
    /* mov (%r1), %r2 */ \
    0x48, 0x8b, BYTE1_LOAD_STORE_reg_to_x86_64(r1, r2), \
            BYTE2_LOAD_STORE_reg_to_x86_64(r1, r2)

#define STORE_to_x86_64(r1, r2) \
    \
    /* mov %r1, (%r2) */ \
    0x48, 0x89, BYTE1_LOAD_STORE_reg_to_x86_64(r1, r2), \
            BYTE2_LOAD_STORE_reg_to_x86_64(r1, r2)

#define JUMP_to_x86_64(addr) \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, byte0_64(addr), byte1_64(addr), byte2_64(addr), \
            byte3_64(addr), byte4_64(addr), byte5_64(addr), \
            byte6_64(addr), byte7_64(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPCOND_to_x86_64(addr, value) \
    \
    /* mov $0, %edx     ; load zero to %edx for comparison */ \
    0xba, 0x00, 0x00, 0x00, 0x00\
    \
    /* cmp %value, %edx ; do the comparison */ \
    0x39, JUMPCOND_reg_to_x86_64(value), \
    \
    /* jg END           ; skip the jump if the value is less than zero */ \
    0x7d, 0x0c, \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, byte0_64(addr), byte1_64(addr), byte2_64(addr), \
            byte3_64(addr), byte4_64(addr), byte5_64(addr), \
            byte6_64(addr), byte7_64(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPLINK_to_x86_64(addr) \
    \
    /* call NEXT_OP     ; pushes address of next instruction */ \
    0xe8, 0x00, 0x00, 0x00, 0x00, \
    \
    /* pop %rcx         ; pop address of next instruction into %rcx (RA) */ \
    0x59, \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, byte0_64(addr), byte1_64(addr), byte2_64(addr), \
            byte3_64(addr), byte4_64(addr), byte5_64(addr), \
            byte6_64(addr), byte7_64(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPADDR_to_x86_64(reg) \
    \
    /* push REG         ; push the jump target from reg to stack */ \
    push_reg_to_x86_64(reg), \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define HALT_to_x86_64(save_addr) \
    \
    /* mov %eax, ($save_addr)   ; Save the accumulator value */ \
    STORE_to_x86_64(ACCUMULATOR, save_addr), \
    \
    /* ret              ; Stop the JIT code and return to the call point */ \
    0xc3

byte MOVE_ADD_SUB_reg_to_x86_64(int r1, int r2);
byte LOADIMM_reg_to_x86_64(int r);
byte BYTE1_LOAD_STORE_reg_to_x86_64(int r1, int r2);
byte BYTE2_LOAD_STORE_reg_to_x86_64(int r1, int r2);
byte push_reg_to_x86_64(int r);

#endif // ncode
