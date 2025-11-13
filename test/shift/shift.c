
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vshift.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define DIVIDER (2u)

Vshift * tb;
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

TEST_GROUP(shift);

uint32_t opened = 0;

TEST_SETUP(shift) 
{
    tb = new Vshift;

    if (opened == 0) 
    {
        tb->trace(trace, 99);
        opened = 1;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
    tb->i_en = 1;
}

TEST_TEAR_DOWN(shift)
{
    delete tb;
}

TEST_GROUP_RUNNER(shift)
{
    RUN_TEST_CASE(shift, test_divider);
}

TEST(shift, test_divider)
{
    trace->open("test_shift.vcd");

    uint32_t result = 0;

    for (int i = 0; i < 8; i ++)
    {
        tb->i_data = 1;
        tick();

        result = result << 1 | 1;
        TEST_ASSERT_EQUAL(result, tb->o_data);
    }
}

// EOF
