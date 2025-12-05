
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vdiff.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

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

    tb->i_en = 0;
    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(diff)
{
    delete tb;
}

TEST_GROUP_RUNNER(diff)
{
    RUN_TEST_CASE(diff, test_no_change);
    RUN_TEST_CASE(diff, test_change);
    RUN_TEST_CASE(diff, test_disable_change);
}

TEST(diff, test_no_change)
{
    for (uint32_t i = 0; i <= 0xFF; i++)
    {
        tb->i_en = 0;
        tb->i_data = i;
        tb->i_rst = 1;
        tick();

        tb->i_en = 1;
        tb->i_rst = 0;

        tick();
        tick();

        TEST_ASSERT_EQUAL(0, tb->o_changed);
    }
}

TEST(diff, test_change)
{
    // trace->open("test_change.vcd");
    for (uint32_t i = 0; i <= 0xFF; i++)
    {
        tb->i_en = 0;
        tb->i_data = 0;

        tick();
        tb->i_en = 1;
        if (i == 0) 
        {
            tb->i_data = 1;
        }
        else 
        {
            tb->i_data = 0;
        }

        tick();
        tick();

        tb->i_data = i;
        tick();

        TEST_ASSERT_EQUAL(1, tb->o_changed);
    }
}

TEST(diff, test_disable_change)
{
    // trace->open("test_disable_change.vcd");
    for (uint32_t i = 0; i <= 0xFF; i++)
    {
        tb->i_en = 0;
        tb->i_data = 0;
        tb->i_rst = 1;

        tick();

        tb->i_en = 1;
        tb->i_rst = 0;

        if (i == 0) 
        {
            tb->i_data = 1;
        }
        else 
        {
            tb->i_data = 0;
        }

        tick();

        // Disable the block
        tb->i_en = 0;
        tick();

        tb->i_data = i;
        tick();

        // No changes should be reported when the block is disabled
        TEST_ASSERT_EQUAL(0, tb->o_changed);
    }
}

// EOF
