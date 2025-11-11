
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vcounter.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define FIFO_DEPTH (256)
#define FIFO_WIDTH (8)

Vcounter * tb;
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

TEST_GROUP(counter);

uint32_t added = 0;

TEST_SETUP(counter) 
{
    tb = new Vcounter;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst  = 1;

    tick(); // 1 clock of Reset

    tb->i_rst  = 0;
    tb->i_en = 1;
}

TEST_TEAR_DOWN(counter)
{
    delete tb;
}

TEST_GROUP_RUNNER(counter)
{
    RUN_TEST_CASE(counter, test_done);
}

TEST(counter, test_done)
{
    uint32_t results[1024] = {0};
    trace->open("test_done.vcd");

    for (int i = 0; i < 1024; i++)
    {
        results[i] = tb->o_line;
        tick();
    }

    for (int i = 256; i < 1024; i++)
    {
        TEST_ASSERT_EQUAL(1, results[i]);
    }
}

// EOF
