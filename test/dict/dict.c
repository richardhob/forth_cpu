
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vdict.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vdict * tb;
extern VerilatedVcdC * trace;

#define ENTRIES (10)
#define KEY_WIDTH (8)
#define KEY_LENGTH (1)

#define VALUE_WIDTH (32)
#define VALUE_LENGTH (1)

enum dict_ops {
    OP_SET = 0,
    OP_GET,
    OP_ENCODE,
    OP_SET_FAST,
    OP_GET_FAST,
    OP_DELETE,
    OP_DELETE_FAST
};

enum dict_states {
    STATE_IDLE = 0,
    STATE_SET,
    STATE_GET,
    STATE_ENCODE,
    STATE_DELETE
};

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
}

TEST_GROUP_RUNNER(dict)
{
    RUN_TEST_CASE(dict, test_set_get);
    RUN_TEST_CASE(dict, test_delete);
    RUN_TEST_CASE(dict, test_reset);
    RUN_TEST_CASE(dict, test_set_single);
    RUN_TEST_CASE(dict, test_get_single);
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

void set(char name, uint32_t value)
{
    tb->i_op = OP_SET;
    tb->i_key[0] = name;
    tb->i_value[0] = value;
    tb->i_index = 0;
    tb->i_ready = 1;

    tick();
    tick();
}

TEST(dict, test_get_single)
{
    set('T', 0xDEADBEEF);

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

void get(const char name)
{
    tb->i_op = OP_GET;
    tb->i_key[0] = name;
    tb->i_value[0] = 0;
    tb->i_index = 0;
    tb->i_ready = 1;

    tick();
    tick();
}

TEST(dict, test_set_get)
{
    set('f', 0x01);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    get('f');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(0, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_value[0]);

    set('s', 0x02);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    get('s');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(1, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(2, tb->o_value[0]);

    set('t', 0x03);

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);

    get('t');

    TEST_ASSERT_EQUAL(1, tb->o_done);
    TEST_ASSERT_EQUAL(2, tb->o_index);
    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(3, tb->o_value[0]);
}

TEST(dict, test_delete)
{
    set('f', 0x01);
    set('s', 0x02);
    set('t', 0x03);

    get('f');
    get('s');
    get('t');
}

// EOF
