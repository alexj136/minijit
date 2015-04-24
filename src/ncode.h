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
    LOADIMM_reg_to_x86_64(r), byte0_32(n), byte1_32(n), byte2_32(n), byte3_32(n)

#define MOVE_to_x86_64(r1, r2) \
    \
    0x89, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define ADD_to_x86_64(r1, r2) \
    \
    0x01, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define SUB_to_x86_64(r1, r2) \
    \
    0x29, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define LOAD_to_x86_64(r1, r2) \
    \
    0x48, 0x8b, BYTE1_LOAD_STORE_reg_to_x86_64(r1, r2), \
            BYTE2_LOAD_STORE_reg_to_x86_64(r1, r2)

#define STORE_to_x86_64(r1, r2) \
    \
    0x48, 0x89, BYTE1_LOAD_STORE_reg_to_x86_64(r1, r2), \
            BYTE2_LOAD_STORE_reg_to_x86_64(r1, r2)

#define JUMPADDR_to_x86_64(addr) \
    \
    /* mov $addr, %rdx */ \
    0x48, 0xba, byte0_64(addr), byte1_64(addr), byte2_64(addr), \
            byte3_64(addr), byte4_64(addr), byte5_64(addr), \
            byte6_64(addr), byte7_64(addr), \
    \
    /* push %rdx */ \
    0x52, \
    \
    /* ret */ \
    0xc3

/*
 * Translate the icode HALT instruction to x86_64.
 */
#define HALT_to_x86_64(SAVE_ADDR) \
    \
    /* Save the accumulator value */ \
    STORE_to_x86_64(ACCUMULATOR, SAVE_ADDR), \
    \
    /* Stop the JIT code and return to the call point */ \
    x86_64_ret

#define x86_64_ret 0xc3

byte MOVE_ADD_SUB_reg_to_x86_64(int r1, int r2);
byte LOADIMM_reg_to_x86_64(int r);
byte BYTE1_LOAD_STORE_reg_to_x86_64(int r1, int r2);
byte BYTE2_LOAD_STORE_reg_to_x86_64(int r1, int r2);

#endif // ncode
