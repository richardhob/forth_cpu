
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vparser.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vparser * tb;
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

TEST_GROUP(parser);

uint32_t added = 0;

TEST_SETUP(parser) 
{
    tb = new Vparser;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(parser)
{
    delete tb;
}

TEST_GROUP_RUNNER(parser)
{
    RUN_TEST_CASE(parser, test_int);
    RUN_TEST_CASE(parser, test_simple);
    RUN_TEST_CASE(parser, test_hex);
    RUN_TEST_CASE(parser, test_opcode);
    // RUN_TEST_CASE(parser, test_invalid);
    // RUN_TEST_CASE(parser, test_whitespace);
}

void send_word(const char * msg, uint32_t len)
{
    tb->i_ready = 1;
    for (int i = 0; i < len; i++)
    {
        tb->i_data = msg[i];
        tick();
    }

    tb->i_wc = 1;
    tick();
    tb->i_wc = 0;
}

enum {
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
};

TEST(parser, test_simple)
{
    trace->open("test_simple.vcd");

    tb->i_en = 1;
    send_word("1", 1);

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(1, tb->o_data);
    TEST_ASSERT_EQUAL(OPCODE_PUSH, tb->o_opcode);
}

TEST(parser, test_int)
{
    trace->open("test_int.vcd");

    tb->i_en = 1;

    char buffer[256] = {0};
    for (int i = 0; i < 10; i++)
    {
        buffer[0] = '0' + i;
        send_word(buffer, 1);

        TEST_ASSERT_EQUAL(0, tb->o_err);
        TEST_ASSERT_EQUAL(i, tb->o_data);
        TEST_ASSERT_EQUAL(OPCODE_PUSH, tb->o_opcode);
    }
}

TEST(parser, test_hex)
{
    trace->open("test_hex.vcd");

    tb->i_en = 1;

    char buffer[256] = {0};
    buffer[0] = '0';
    buffer[1] = 'x';
    for (int i = 0; i < 10; i++)
    {
        buffer[2] = '0' + i;
        send_word(buffer, 3);

        TEST_ASSERT_EQUAL(0, tb->o_err);
        TEST_ASSERT_EQUAL(i, tb->o_data);
        TEST_ASSERT_EQUAL(OPCODE_PUSH, tb->o_opcode);
    }
}

TEST(parser, test_opcode)
{
    trace->open("test_opcode.vcd");

    tb->i_en = 1;
    send_word(".", 1);

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(0, tb->o_data);
    TEST_ASSERT_EQUAL(OPCODE_POP, tb->o_opcode);
}

// EOF
