
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
VerilatedVcdC * trace;

enum {
    CMD_NONE = 0,
    CMD_PUSH = 1,
    CMD_POP  = 2
};

uint32_t g_tick = 1;
void tick()
{
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

TEST_SETUP(fifo) 
{
    g_tick = 1;

    tb = new Vfifo;
    trace = new VerilatedVcdC;
    tb->trace(trace, 99);

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
    trace->close();
    delete tb;
}

TEST_GROUP_RUNNER(fifo)
{
    RUN_TEST_CASE(fifo, test_push_and_pop);
}

TEST(fifo, test_push_and_pop)
{
    trace->open("test_fail.vcd");

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

// EOF
