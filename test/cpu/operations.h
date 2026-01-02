
#ifndef CPU_OPERATIONS_H_
#define CPU_OPERATIONS_H_

#include <stdint.h>

typedef enum {
    OPCODE_IDLE = 0,
    OPCODE_PUSH,
    OPCODE_POP,
    OPCODE_ADD,
    OPCODE_SUBTRACT,
    OPCODE_MULTIPLY,
    OPCODE_DIVIDE,
    OPCODE_DUP,
    OPCODE_ROT,
    OPCODE_SWAP
} cpu_opcodes;

typedef enum {
    ERROR_OK = 0,
    ERROR_INVALID_OPCODE,
    ERROR_STACK_TOO_SMALL,
    ERROR_STACK_TOO_BIG,
    ERROR_DIVIDE_BY_ZERO
} cpu_errors;

void op_idle(void);
void op_push(uint32_t value);
void op_pop(void);
void op_add(void);
void op_sub(void);
void op_mul(void);
void op_div(void);
void op_dup(void);
void op_rot(void);
void op_swap(void);

#endif  // CPU_OPERATIONS

// EOF
