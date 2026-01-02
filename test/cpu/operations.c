
#include "operations.h"

#include "Vcpu.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

extern Vcpu * tb;
extern void tick(void);

void _opcode(cpu_opcodes op)
{
    tb->i_opcode = op;

    tb->i_ready = 1;
    tick();
    tb->i_ready = 0;
}

void op_idle(void)
{
    _opcode(OPCODE_IDLE);
}

void op_push(uint32_t value)
{
    tb->i_data = value;
    _opcode(OPCODE_PUSH);
}

void op_pop(void)
{
    _opcode(OPCODE_POP);
}

void op_add(void)
{
    _opcode(OPCODE_ADD);
}

void op_sub(void)
{
    _opcode(OPCODE_SUBTRACT);
}

void op_mul(void)
{
    _opcode(OPCODE_MULTIPLY);
}

void op_div(void)
{
    _opcode(OPCODE_DIVIDE);
}

void op_dup(void)
{
    _opcode(OPCODE_DUP);
}

void op_rot(void)
{
    _opcode(OPCODE_ROT);
}

void op_swap(void)
{
    _opcode(OPCODE_SWAP);
}

// EOF
