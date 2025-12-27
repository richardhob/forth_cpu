
#include "apis.h"

// Must be defined _SOMEWHERE_
extern Vdict * tb;
extern void tick();

void op_set(char name, uint32_t value)
{
    tb->i_op = OP_SET;
    tb->i_key[0] = name;
    tb->i_value[0] = value;
    tb->i_ready = 1;

    tick();
    tick();
}

void op_get(const char name)
{
    tb->i_op = OP_GET;
    tb->i_key[0] = name;
    tb->i_ready = 1;

    tick();
    tick();
}

void op_delete(char name)
{
    tb->i_op = OP_DELETE;
    tb->i_key[0] = name;
    tb->i_ready = 1;

    tick();
    tick();
}

void op_encode(char name)
{
    tb->i_op = OP_ENCODE;
    tb->i_key[0] = name;
    tb->i_ready = 1;

    tick();
    tick();
    tick();
}

void op_set_fast(uint8_t index, char name, uint32_t value)
{
    tb->i_op = OP_SET_FAST;
    tb->i_index = index;
    tb->i_key[0] = name;
    tb->i_value[0] = value;
    tb->i_ready = 1;

    tick();
}

void op_get_fast(uint8_t index)
{
    tb->i_op = OP_GET_FAST;
    tb->i_index = index;
    tb->i_ready = 1;

    tick();
}

void op_delete_fast(uint8_t index)
{
    tb->i_op = OP_DELETE_FAST;
    tb->i_index = index;
    tb->i_ready = 1;

    tick();
}

// EOF
