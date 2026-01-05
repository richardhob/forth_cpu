
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "VtokenizerL.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

VtokenizerL * tb;
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

TEST_GROUP(tokenizerL);

uint32_t added = 0;

TEST_SETUP(tokenizerL) 
{
    tb = new VtokenizerL;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(tokenizerL)
{
    delete tb;
}

TEST_GROUP_RUNNER(tokenizerL)
{
    RUN_TEST_CASE(tokenizerL, test_simple);
}

void write_message(const char * msg, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) tb->i_line[i] = msg[i];
    tb->i_len = len;
    tb->i_ready = 1;
    tick();
    tb->i_ready = 0;
}

TEST(tokenizerL, test_simple)
{
    trace->open("test_simple.vcd");

    tb->i_en = 1;
    write_message("a b c\n", 6);

    TEST_ASSERT_EQUAL(0, tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);

    TEST_ASSERT_EQUAL(6, tb->d_len);
    TEST_ASSERT_EQUAL(0, tb->d_index);

    tb->i_next = 1;
    tick();

    TEST_ASSERT_EQUAL('a', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(1, tb->d_index);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL(' ', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(2, tb->d_index);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(1, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL('b', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(3, tb->d_index);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL(' ', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(4, tb->d_index);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(1, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL('c', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(5, tb->d_index);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL('\n', tb->o_data);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
    TEST_ASSERT_EQUAL(6, tb->d_index);
    TEST_ASSERT_EQUAL(1, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);

    tick();

    TEST_ASSERT_EQUAL(0, tb->o_data_ready);
    TEST_ASSERT_EQUAL(0, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);
    TEST_ASSERT_EQUAL(0, tb->d_state);
    TEST_ASSERT_EQUAL(0, tb->d_len);
    TEST_ASSERT_EQUAL(0, tb->d_index);
}

// EOF
