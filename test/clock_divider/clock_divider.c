
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vclock_divider.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define DIVIDER (2u)

Vclock_divider * tb;
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

TEST_GROUP(clock_divider);

uint32_t opened = 0;

TEST_SETUP(clock_divider) 
{
    tb = new Vclock_divider;

    if (opened == 0) 
    {
        tb->trace(trace, 99);
        opened = 1;
    }
}

TEST_TEAR_DOWN(clock_divider)
{
    delete tb;
}

TEST_GROUP_RUNNER(clock_divider)
{
    RUN_TEST_CASE(clock_divider, test_hold_reset);
    RUN_TEST_CASE(clock_divider, test_divider);
}

TEST(clock_divider, test_hold_reset)
{
    // trace->open("test_hold_reset.vcd");
    tb->i_rst = 1;
    uint32_t output_clocks[10] = {0};

    for(int i = 0; i < 10; i++)
    {
        tick();
        output_clocks[i] = tb->o_clk;
    }

    for (int i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL(0, output_clocks[i]);
    }
}

TEST(clock_divider, test_divider)
{
    // trace->open("test_divider.vcd");
    uint32_t output_state = 0;

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;

    TEST_ASSERT_EQUAL(output_state, tb->o_clk);

    for (int i = 1; i < 33; i ++)
    {
        tick();
        if (i % (DIVIDER + 1) == 0)
        {
            TEST_ASSERT_EQUAL(!output_state, tb->o_clk);
            output_state = !output_state;
        }
    }
}

// EOF
