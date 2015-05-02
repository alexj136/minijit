#ifndef ncode
#define ncode

typedef unsigned char byte;

/*
 * Set up execution - set register values appropriately. Requires the address of
 * the stack for the native code to use.
 */
#define x86_64_preamble(stack_addr) \
    LOADIMM_to_x86_64(0, ACCUMULATOR), \
    LOADIMM_to_x86_64(0, TEMPORARY), \
    LOADIMM_to_x86_64(0, RETURN_ADDRESS), \
    0x48, 0xBE, byte0_64(stack_addr), byte1_64(stack_addr), \
            byte2_64(stack_addr), byte3_64(stack_addr), byte4_64(stack_addr), \
            byte5_64(stack_addr), byte6_64(stack_addr), byte7_64(stack_addr), \
    0x48, 0xBF, byte0_64(stack_addr), byte1_64(stack_addr), \
            byte2_64(stack_addr), byte3_64(stack_addr), byte4_64(stack_addr), \
            byte5_64(stack_addr), byte6_64(stack_addr), byte7_64(stack_addr) \

#define x86_64_preamble_size ((3 * (LOADIMM_x86_64_size)) + 20)

/*
 * Macros to convert a 32-bit type to a 4 element list of comma-delimited bytes
 */
#define byte0_32(n) ((byte) ((((uint32_t) n) & 0x000000FF) >>  0))
#define byte1_32(n) ((byte) ((((uint32_t) n) & 0x0000FF00) >>  8))
#define byte2_32(n) ((byte) ((((uint32_t) n) & 0x00FF0000) >> 16))
#define byte3_32(n) ((byte) ((((uint32_t) n) & 0xFF000000) >> 24))
#define t32_to_bytes(n) \
    byte0_32(n), byte1_32(n), byte2_32(n), byte3_32(n) \

/*
 * Macros to convert a 64-bit type to an 8 element list of comma-delimited bytes
 */
#define byte0_64(n) ((byte) ((((uint64_t) n) & 0x00000000000000FF) >>  0))
#define byte1_64(n) ((byte) ((((uint64_t) n) & 0x000000000000FF00) >>  8))
#define byte2_64(n) ((byte) ((((uint64_t) n) & 0x0000000000FF0000) >> 16))
#define byte3_64(n) ((byte) ((((uint64_t) n) & 0x00000000FF000000) >> 24))
#define byte4_64(n) ((byte) ((((uint64_t) n) & 0x000000FF00000000) >> 32))
#define byte5_64(n) ((byte) ((((uint64_t) n) & 0x0000FF0000000000) >> 40))
#define byte6_64(n) ((byte) ((((uint64_t) n) & 0x00FF000000000000) >> 48))
#define byte7_64(n) ((byte) ((((uint64_t) n) & 0xFF00000000000000) >> 56))
#define t64_to_bytes(n) \
    byte0_64(n), byte1_64(n), byte2_64(n), byte3_64(n), \
    byte4_64(n), byte5_64(n), byte6_64(n), byte7_64(n) \

#define LOADIMM_to_x86_64(n, r) \
    \
    /* mov $n, %r */ \
    LOADIMM_reg_to_x86_64(r), t32_to_bytes(n)

#define LOADIMM_x86_64_size 5

#define MOVE_to_x86_64(r1, r2) \
    \
    /* mov %r1, %r2 */ \
    0x89, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define MOVE_x86_64_size 2

#define ADD_to_x86_64(r1, r2) \
    \
    /* add %r1, %r2 */ \
    0x01, MOVE_ADD_SUB_reg_to_x86_64(r1, r2)

#define ADD_x86_64_size 2

#define SUB_to_x86_64(r1, r2) \
    \
    /* sub %r1, %r2 */ \
    0x29, MOVE_ADD_SUB_reg_to_x86_64(r2, r1)

#define SUB_x86_64_size 2

#define LOAD_to_x86_64(r1, r2) \
    \
    /* mov (%r1), %r2 */ \
    0x48, 0x8b, LOAD_STORE_reg_to_x86_64(r1, r2)

#define LOAD_x86_64_size 3

#define STORE_to_x86_64(r1, r2) \
    \
    /* mov %r1, (%r2) */ \
    0x48, 0x89, LOAD_STORE_reg_to_x86_64(r2, r1)

#define STORE_x86_64_size 3

#define JUMP_to_x86_64(addr) \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, t64_to_bytes(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMP_x86_64_size 12
#define JUMP_x86_64_address_offset 2

#define JUMPCOND_to_x86_64(addr, value) \
    \
    /* mov $0, %edx     ; load zero to %edx for comparison */ \
    0xba, 0x00, 0x00, 0x00, 0x00, \
    \
    /* cmp %value, %edx ; do the comparison */ \
    0x39, JUMPCOND_reg_to_x86_64(value), \
    \
    /* jg END           ; skip the jump if the value is less than zero */ \
    0x7d, 0x0c, \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, t64_to_bytes(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPCOND_x86_64_size 21
#define JUMPCOND_x86_64_address_offset 11

#define JUMPLINK_to_x86_64(addr) \
    \
    /* call NEXT_OP     ; pushes address of next instruction */ \
    0xe8, 0x00, 0x00, 0x00, 0x00, \
    \
    /* pop %rcx         ; pop address of next instruction into %rcx (RA) */ \
    0x59, \
    \
    /* mov $addr, %rdx  ; load the target address */ \
    0x48, 0xba, t64_to_bytes(addr), \
    \
    /* push %rdx        ; push the target address */ \
    0x52, \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPLINK_x86_64_size 18
#define JUMPLINK_x86_64_address_offset 8

#define JUMPADDR_to_x86_64(reg) \
    \
    /* push REG         ; push the jump target from reg to stack */ \
    push_reg_to_x86_64(reg), \
    \
    /* ret              ; pop and jump to the target address */ \
    0xc3

#define JUMPADDR_x86_64_size 2

#define HALT_to_x86_64(save_addr) \
    \
    /* mov $save_addr, %rdx ; load the save address */ \
    0x48, 0xba, t64_to_bytes(save_addr), \
    \
    /* mov %eax, (%rdx) ; Store the accumulator value at save_addr */ \
    0x89, 0x02, \
    \
    /* ret              ; Stop the JIT code and return to the call point */ \
    0xc3

#define HALT_x86_64_size 13

byte MOVE_ADD_SUB_reg_to_x86_64(int r1, int r2);
byte LOADIMM_reg_to_x86_64(int r);
byte LOAD_STORE_reg_to_x86_64(int r1, int r2);
byte push_reg_to_x86_64(int r);
byte *allocate_executable(byte *memory, size_t size);
void release_executable(byte *exec_mem, size_t size);

#endif // ncode
