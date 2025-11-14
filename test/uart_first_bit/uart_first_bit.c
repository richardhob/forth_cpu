
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_first_bit.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define OSR        (16u)
#define PULSE_WIDTH (2u)

Vuart_first_bit * tb;
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

TEST_GROUP(uart_first_bit);

uint32_t opened = 0;

TEST_SETUP(uart_first_bit) 
{
    tb = new Vuart_first_bit;

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

TEST_TEAR_DOWN(uart_first_bit)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_first_bit)
{
    RUN_TEST_CASE(uart_first_bit, test_valid);
    RUN_TEST_CASE(uart_first_bit, test_pulse_1);
}

TEST(uart_first_bit, test_valid)
{
    // trace->open("test_valid.vcd");

    uint8_t output[OSR + 2] = {0};

    tb->i_rx = 1;

    for (int i = 0; i <= (OSR+1); i++)
    {
        output[i] = tb->o_found;
        tick();
    }

    for (int i = 0; i <= (OSR/2); i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, output[i], "Debounce");
    }

    for (int i = (OSR/2) + 1; i < (OSR/2) + 1 + PULSE_WIDTH; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(1, output[i], "Bit HI");
    }

    for (int i = (OSR/2) + 1 + PULSE_WIDTH; i < OSR + 2; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, output[i], "Bit Low");
    }

}

TEST(uart_first_bit, test_pulse_1)
{
    // trace->open("test_pulse_1.vcd");

    uint8_t output[OSR+2] = {0};

    tb->i_rx = 1;
    output[0] = tb->o_found;
    tick();
    tb->i_rx = 0;
    output[1] = tb->o_found;
    tick();

    for (int i = 2; i < OSR+2; i++)
    {
        output[i] = tb->o_found;
        tick();
    }

    for (int i = 0; i < OSR+2; i++)
    {
        TEST_ASSERT_EQUAL(0, output[i]);
    }
}

// EOF
