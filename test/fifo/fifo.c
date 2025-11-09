
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vfifo.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define FIFO_DEPTH (256)
#define FIFO_WIDTH (8)

Vfifo * tb;
extern VerilatedVcdC * trace;

enum {
    CMD_NONE = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2
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

    tb->i_reset  = 1;
    tb->i_enable = 1;
    tb->i_cmd    = CMD_NONE;
    tb->i_data   = 0x0;

    tick(); // 1 clock of Reset

    tb->i_reset  = 0;
    tb->i_enable = 1;
    tb->i_cmd    = CMD_NONE;
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

    tb->i_cmd = CMD_PUSH;
    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_data = i;
        tick();
    }

    tb->i_data = 0;
    tb->i_cmd = CMD_POP;
    tick();

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        TEST_ASSERT_EQUAL(i, tb->o_data);
        tick();
    }
}

TEST(fifo, test_push_and_none)
{
    // trace->open("test_push_and_none.vcd");

    tb->i_cmd = CMD_PUSH;
    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_data = i;
        tick();
    }

    tb->i_data = 0;
    tb->i_cmd = CMD_NONE;
    tick();

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        TEST_ASSERT_EQUAL(0, tb->o_data);
        tick();
    }
}

TEST(fifo, test_none_and_pop)
{
    // trace->open("test_none_and_pop");

    tb->i_cmd = CMD_NONE;
    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        tb->i_data = i;
        tick();
    }

    tb->i_data = 0;
    tb->i_cmd = CMD_POP;
    tick();

    for (int i = 0; i < FIFO_DEPTH; i++)
    {
        TEST_ASSERT_EQUAL(0, tb->o_data);
        tick();
    }
}

// EOF
