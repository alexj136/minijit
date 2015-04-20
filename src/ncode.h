#ifndef ncode
#define ncode

typedef unsigned char byte;

#define LOADIMM_to_x86_64(n, r) \
    \
    LOADIMM_reg_to_x86_64(r), byte0(n), byte1(n), byte2(n), byte3(n)

#define byte0(n) ((byte)((n & 0x000000FF) >>  0))
#define byte1(n) ((byte)((n & 0x0000FF00) >>  8))
#define byte2(n) ((byte)((n & 0x00FF0000) >> 16))
#define byte3(n) ((byte)((n & 0xFF000000) >> 24))

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
