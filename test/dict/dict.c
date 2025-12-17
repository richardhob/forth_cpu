
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
    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(dict)
{
}

TEST_GROUP_RUNNER(dict)
{
    RUN_TEST_CASE(dict, test_reset);
}

TEST(dict, test_reset)
{
    TEST_FAIL();
}

// EOF
