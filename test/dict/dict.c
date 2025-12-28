
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vdict.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#include "apis.h"

Vdict * tb;
extern VerilatedVcdC * trace;

void tick()
{
    static uint32_t g_tick = 1;
    tb->eval();

    // 2ns Before the tick
    if (trace) trace->dump(g_tick * 10 - 2);

    tb->i_clk = 1;
    tb->eval();

    // 10ns Tick 
    if (trace) trace->dump(g_tick * 10);
    
    tb->i_clk = 0;
    tb->eval();

    // Trailing Edge
    if (trace) 
    {
        trace->dump(g_tick * 10 + 5);
        trace->flush();
    }

    g_tick = g_tick + 1;
}

TEST_GROUP(dict);

TEST_SETUP(dict) 
{
    tb->i_en = 0;
    tb->i_op = STATE_IDLE;
    tb->i_index = 0;
    tb->i_ready = 0;

    for (int i = 0; i < KEY_LENGTH; i++)
    {
        tb->i_key[i] = 0;
    }

    for (int i = 0; i < VALUE_LENGTH; i++)
    {
        tb->i_value[i] = 0;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
    tb->i_en = 1;
}

TEST_TEAR_DOWN(dict)
{
    for (int i = 0; i < ENTRIES; i++)
    {
        op_delete_fast(i);
    }
}

TEST_GROUP_RUNNER(dict)
{
    RUN_TEST_CASE(dict, test_reset);
    RUN_TEST_CASE(dict, test_set_single);
    RUN_TEST_CASE(dict, test_get_single);
    RUN_TEST_CASE(dict, test_set_get);
    RUN_TEST_CASE(dict, test_delete);
    RUN_TEST_CASE(dict, test_delete_forward);
    RUN_TEST_CASE(dict, test_delete_backward);
    RUN_TEST_CASE(dict, test_encode);
    RUN_TEST_CASE(dict, test_set_fast);
    RUN_TEST_CASE(dict, test_get_fast);
    RUN_TEST_CASE(dict, test_delete_fast);
    RUN_TEST_CASE(dict, test_set_full);

    // TODO
    RUN_TEST_CASE(dict, test_get_miss_empty);
    RUN_TEST_CASE(dict, test_get_miss_full);
    RUN_TEST_CASE(dict, test_delete_miss);
    RUN_TEST_CASE(dict, test_encode_miss);
    RUN_TEST_CASE(dict, test_duplicates)
}

TEST(dict, test_reset)
{
    TEST_ASSERT_EQUAL(0, tb->o_done);

    for (int i = 0; i < VALUE_LENGTH; i++) 
    {
        TEST_ASSERT_EQUAL(0, tb->o_value[i]);
    }

    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(STATE_IDLE, tb->d_state);
}

TEST(dict, test_set_single)
{
    tb->i_key[0] = 'T';
    tb->i_value[0] = 0xDEADBEEF;
    tb->i_op = OP_SET;
    tb->i_ready = 1;

    tick();
    TEST_ASSERT_EQUAL(STATE_SET, tb->d_state);
    tick();

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(STATE_IDLE, tb->d_state);

    TEST_ASSERT_EQUAL(0, tb->o_value[0]);
}

TEST(dict, test_get_single)
{
    op_set('T', 0xDEADBEEF);

    tb->i_op = OP_GET;
    tb->i_key[0] = 'T';
    tb->i_value[0] = 0;
    tb->i_ready = 1;

    tick();
    TEST_ASSERT_EQUAL(STATE_GET, tb->d_state);
    tick();

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(STATE_IDLE, tb->d_state);
    TEST_ASSERT_EQUAL(0xDEADBEEF, tb->o_value[0]);
}

TEST(dict, test_set_get)
{
    op_set('f', 0x01);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    op_get('f');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);

    op_set('s', 0x02);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    op_get('s');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);

    op_set('t', 0x03);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    op_get('t');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(3, tb->o_value[0]);
}

TEST(dict, test_delete)
{
    op_set('f', 0x01);
    op_get('f');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);

    tb->i_op = OP_DELETE;
    tb->i_key[0] = 'f';
    tb->i_value[0] = 0;
    tb->i_index = 0;
    tb->i_ready = 1;

    tick();
    TEST_ASSERT_EQUAL(STATE_DELETE, tb->d_state);
    tick();

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);
}

TEST(dict, test_delete_forward)
{
    op_set('a', 1);
    op_set('b', 2);
    op_set('c', 3);

    op_delete('a');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);

    op_delete('b');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);

    op_delete('c');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(3, tb->o_value[0]);
}

TEST(dict, test_delete_backward)
{
    op_set('a', 1);
    op_set('b', 2);
    op_set('c', 3);

    op_delete('c');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(3, tb->o_value[0]);

    op_delete('b');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);

    op_delete('a');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);
}

TEST(dict, test_encode)
{
    op_set('a', 1);
    op_set('b', 2);
    op_set('c', 3);

    tb->i_op = OP_ENCODE;
    tb->i_key[0] = 'b';
    tb->i_ready = 1;

    tick();
    TEST_ASSERT_EQUAL(STATE_ENCODE, tb->d_state);
    tick();

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
}

TEST(dict, test_set_fast)
{
    op_set_fast(0, 'a', 0x111);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    op_set_fast(1, 'b', 0x222);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    op_set_fast(2, 'c', 0x333);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
}

TEST(dict, test_get_fast)
{
    op_set('a', 1);
    op_set('b', 2);
    op_set('c', 3);

    op_get_fast(0);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);

    op_get_fast(1);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);

    op_get_fast(2);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(3, tb->o_value[0]);
}

TEST(dict, test_delete_fast)
{
    op_set('a', 1);
    op_set('b', 2);
    op_set('c', 3);
    op_set('d', 4);

    op_delete_fast(1);
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);
}

TEST(dict, test_set_full)
{
    for (int i = 0; i < ENTRIES; i++)
    {
        op_set('0' + i, 0x100 + i);
    }

    op_set('?', 0x123);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(dict, test_get_miss_empty)
{
    op_get('?');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_value[0]);
    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(dict, test_get_miss_full)
{
    for (int i = 0; i < ENTRIES; i++)
    {
        op_set('0' + i, 0x100 + i);
    }

    op_get('?');
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_value[0]);
    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(dict, test_delete_miss)
{
    op_delete('?');
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(0, tb->o_value[0]);
    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(dict, test_encode_miss)
{
    op_encode('?');
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->i_index);
    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(dict, test_duplicates)
{
    for (int i = 0; i < ENTRIES; i++)
    {
        op_set_fast(i, 'X', i);
    }

    op_get('X');
    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(9, tb->i_index);
    TEST_ASSERT_EQUAL(9, tb->i_value[0]);
    TEST_ASSERT_EQUAL(0, tb->o_err);
}

// EOF
