
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_stop_bits.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vuart_stop_bits * tb;
extern VerilatedVcdC * trace;

#define OSR       (16u)
#define STOP_BITS  (2u)

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

TEST_GROUP(uart_stop_bits);

uint32_t opened = 0;

TEST_SETUP(uart_stop_bits) 
{
    tb = new Vuart_stop_bits;

    if (opened == 0) 
    {
        tb->trace(trace, 99);
        opened = 1;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(uart_stop_bits)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_stop_bits)
{
    RUN_TEST_CASE(uart_stop_bits, test_valid);
}

TEST(uart_stop_bits, test_valid)
{
    trace->open("test_valid.vcd");

    tb->i_en = 1;
    tb->i_start = 1;
    tb->i_rx = 1;

    tick();

    // The '+2' is (I think) because of how the `uart_data_bits` module does its
    // compare. Which should be fine for the STOP bits, but might not be fine
    // for the data bits ... may be worth investigating?
    for (uint32_t i = 0; i < (OSR * STOP_BITS) + 2; i++)
    {
        tick();
        TEST_ASSERT_EQUAL(0, tb->o_ready);
    }

    tick();
    TEST_ASSERT_EQUAL(1, tb->o_ready);
}

// EOF
