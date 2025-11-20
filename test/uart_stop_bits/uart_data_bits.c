
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_data_bits.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define DATA_BITS (8u)

Vuart_data_bits * tb;
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

TEST_GROUP(uart_data_bits);

uint32_t opened = 0;

TEST_SETUP(uart_data_bits) 
{
    tb = new Vuart_data_bits;

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

TEST_TEAR_DOWN(uart_data_bits)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_data_bits)
{
    RUN_TEST_CASE(uart_data_bits, test_valid_bytes);
}

void valid_byte(uint8_t data)
{
    for (int i = 0; i < DATA_BITS; i++)
    {
        tb->i_rx = ((data & (1 << i)) >> i);
        tick();
    }
}

TEST(uart_data_bits, test_valid_bytes)
{
    trace->open("test_f.vcd");

    tb->i_start = 1;
    for (uint32_t i = 0; i < 0xFF; i++)
    {
        valid_byte(i);
        TEST_ASSERT_EQUAL(i, tb->o_data);
    }
}

// EOF
