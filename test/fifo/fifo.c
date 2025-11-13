
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vfifo.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define FIFO_WIDTH (8)
#define FIFO_DEPTH (256)

Vfifo * tb;
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

TEST_GROUP(fifo);

uint32_t added = 0;

TEST_SETUP(fifo) 
{
    tb = new Vfifo;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;
    tb->i_en     = 1;
    tb->i_set    = 0;
    tb->i_get    = 0;
    tb->i_data   = 0x0;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
    tb->i_en     = 1;
    tb->i_set    = 0;
    tb->i_get    = 0;
    tb->i_data   = 0x0;
}

TEST_TEAR_DOWN(fifo)
{
    delete tb;
}

TEST_GROUP_RUNNER(fifo)
{
    RUN_TEST_CASE(fifo, test_push_and_pop);
    RUN_TEST_CASE(fifo, test_push_and_none);
    RUN_TEST_CASE(fifo, test_none_and_pop);
}

TEST(fifo, test_push_and_pop)
{
    // trace->open("test_push_and_pop.vcd");

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_data = i;
        tb->i_set = 1;

        tick();

        tb->i_set = 0;
        TEST_ASSERT_EQUAL(1, tb->o_set);

        tick();

        TEST_ASSERT_EQUAL(0, tb->o_set);
    }

    tb->i_data = 0;
    tb->i_set = 0;

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_get = 1;

        tick();

        tb->i_get = 0;
        TEST_ASSERT_EQUAL(i, tb->o_data);
        TEST_ASSERT_EQUAL(1, tb->o_get);

        tick();

        TEST_ASSERT_EQUAL(0, tb->o_get);
    }
}

TEST(fifo, test_push_and_none)
{
    // trace->open("test_push_and_none.vcd");

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_set = 1;
        tb->i_data = i;
        tick();

        tb->i_set = 0;
        TEST_ASSERT_EQUAL(1, tb->o_set);

        tick();

        TEST_ASSERT_EQUAL(0, tb->o_set);
    }

    tb->i_data = 0;
    tb->i_set = 0;
    tb->i_get = 0;

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        TEST_ASSERT_EQUAL(0, tb->o_data);
        TEST_ASSERT_EQUAL(0, tb->o_get);
        tick();
    }
}

TEST(fifo, test_none_and_pop)
{
    // trace->open("test_none_and_pop");

    tb->i_set = 0;
    tb->i_get = 0;

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_data = i;
        tick();
    }

    tb->i_data = 0;

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_get = 1;
        tick();

        tb->i_get = 0;
        TEST_ASSERT_EQUAL(1, tb->o_get);
        TEST_ASSERT_EQUAL(0, tb->o_data);

        tick();

        TEST_ASSERT_EQUAL(0, tb->o_get);
    }
}

// EOF
