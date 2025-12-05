
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vdiff.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define FIFO_WIDTH (8)
#define FIFO_DEPTH (256)

Vdiff * tb;
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

TEST_GROUP(diff);

uint32_t added = 0;

TEST_SETUP(diff) 
{
    tb = new Vdiff;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }
}

TEST_TEAR_DOWN(diff)
{
    delete tb;
}

TEST_GROUP_RUNNER(diff)
{
    RUN_TEST_CASE(diff, test_fail);
}

TEST(diff, test_fail)
{
    TEST_FAIL();
}

// EOF
