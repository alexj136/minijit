#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include "util.h"
#include "icode.h"
#include "ncode.h"

byte MOVE_ADD_SUB_reg_lookup_table[6][6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESI     EDI     ECX     EIP     EAX     EBX */
    /* SP  ESI */ { 0xF6,   0xF7,   0xF1,   0x00,   0xF0,   0xF3 },
    /* FP  EDI */ { 0xFE,   0xFF,   0xF9,   0x00,   0xF8,   0xFB },
    /* RA  ECX */ { 0xCE,   0xCF,   0xC9,   0x00,   0xC8,   0xCB },
    /* PC  EIP */ { 0x00,   0x00,   0x00,   0x00,   0x00,   0x00 },
    /* ACC EAX */ { 0xC6,   0xC7,   0xC1,   0x00,   0xC0,   0xC3 },
    /* TMP EBX */ { 0xDE,   0xDF,   0xD9,   0x00,   0xD8,   0xDB }
};

byte LOADIMM_reg_lookup_table[6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESI     EDI     ECX     EIP     EAX     EBX */
                    0xBE,   0xBF,   0xB9,   0x00,   0xB8,   0xBB
};

byte LOAD_STORE_reg_lookup_table[6][6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     RSI     RDI     RCX     RIP     RAX     RBX */
    /* SP  RSI */ { 0x36,   0x3E,   0x0E,   0x00,   0x06,   0x1E },
    /* FP  RDI */ { 0x37,   0x3F,   0x0F,   0x00,   0x07,   0x1F },
    /* RA  RCX */ { 0x31,   0x39,   0x09,   0x00,   0x01,   0x19 },
    /* PC  RIP */ { 0x00,   0x00,   0x00,   0x00,   0x00,   0x00 },
    /* ACC RAX */ { 0x30,   0x38,   0x08,   0x00,   0x00,   0x18 },
    /* TMP RBX */ { 0x33,   0x3B,   0x0B,   0x00,   0x03,   0x1B }
};

byte push_reg_lookup_table[6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     RSI     RDI     RCX     RIP     RAX     RBX */
                    0x56,   0x57,   0x51,   0x00,   0x50,   0x53
};

byte JUMPCOND_reg_lookup_table[6] = {
    /* (Virtual)    SP      FP      RA      PC      ACC     TMP */
    /* (Native)     ESI     EDI     ECX     EIP     EAX     EBX */
                    0xF2,   0xFA,   0xCA,   0x00,   0xC2,   0xDA
};

#define CHECK_ICODE_REG(r) \
    do { \
        if((r > TEMPORARY) || (r < STACK_POINTER)) { \
            ERROR("Invalid register"); \
        } \
        else if(r == PROGRAM_COUNTER) { \
            ERROR("Cannot modify or use program counter register"); \
        } \
    } while(0)

byte MOVE_ADD_SUB_reg_to_x86_64(int r1, int r2) {
    CHECK_ICODE_REG(r1);
    CHECK_ICODE_REG(r2);
    return MOVE_ADD_SUB_reg_lookup_table[r1][r2];
}

byte LOADIMM_reg_to_x86_64(int r) {
    CHECK_ICODE_REG(r);
    return LOADIMM_reg_lookup_table[r];
}

byte LOAD_STORE_reg_to_x86_64(int r1, int r2) {
    CHECK_ICODE_REG(r1);
    CHECK_ICODE_REG(r2);
    return LOAD_STORE_reg_lookup_table[r1][r2];
}

byte push_reg_to_x86_64(int r) {
    CHECK_ICODE_REG(r);
    return push_reg_lookup_table[r];
}

byte JUMPCOND_reg_to_x86_64(int r) {
    CHECK_ICODE_REG(r);
    return JUMPCOND_reg_lookup_table[r];
}

/*
 * The following functions are used to write native code commands to byte
 * buffers. They assume that the command will fit in the target buffer at the
 * desired location and so will have undefined behaviour if the target buffer
 * is too small.
 */

void write_x86_64_preamble(byte *target, byte *stack_addr, int *save_addr) {
    byte code[] = { x86_64_preamble(stack_addr, save_addr) };
	memcpy(target, code, x86_64_preamble_size);
    return;
}

void write_x86_64_LOADIMM(byte *target, int value, int reg) {
    byte code[] = { LOADIMM_to_x86_64(value, reg) };
	memcpy(target, code, LOADIMM_x86_64_size);
    return;
}

/*
 * Copy the given byte buffer into executable memory and return a pointer to the
 * executable memory. The returned buffer has size equal to the given buffer
 * size.
 */
byte *allocate_executable(byte *memory, size_t size) {

	void *exec_mem = mmap(NULL, size,
		PROT_WRITE | PROT_EXEC,	MAP_ANON | MAP_PRIVATE, -1, 0);

	memcpy(exec_mem, memory, size);

    return (byte *) exec_mem;
}

void release_executable(byte *exec_mem, size_t size) {
	munmap(exec_mem, size);
}
