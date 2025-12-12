
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vtokenizer.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define START     (1u)
#define DATA      (8u)
#define STOP      (2u)
#define COOLDOWN  (1u)

#define OSR      (16u)

Vtokenizer * tb;
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

TEST_GROUP(tokenizer);

uint32_t added = 0;

TEST_SETUP(tokenizer) 
{
    tb = new Vtokenizer;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(tokenizer)
{
    delete tb;
}

TEST_GROUP_RUNNER(tokenizer)
{
    RUN_TEST_CASE(tokenizer, test_eol);
}

TEST(tokenizer, test_eol)
{
    trace->open("test_eol.vcd");

    tb->i_en = 1;

    tick();

    tb->i_data = 0xFF;
    tb->i_ready = 1;

    tick();
    tick();

    tb->i_ready = 0;

    tick();
    tick();

    tb->i_data = '\n';
    tb->i_ready = 1;

    tick();
    tick();

    tb->i_ready = 0;
    tick();

    tb->i_next = 1;

    tick();
    tick();

    tb->i_next = 0;

    tick();
    tick();

    tb->i_next = 1;

    tick();

    TEST_ASSERT_EQUAL(1, tb->o_eol);
    TEST_ASSERT_EQUAL(0, tb->o_wc);
    TEST_ASSERT_EQUAL(1, tb->o_data_ready);
}

// EOF
