
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
    RUN_TEST_CASE(uart_first_bit, test_glitch_then_high)
    RUN_TEST_CASE(uart_first_bit, test_constant_hi);
    RUN_TEST_CASE(uart_first_bit, test_pulse_below_osr_div_2)
    RUN_TEST_CASE(uart_first_bit, test_constant_hi_disable)
}

TEST(uart_first_bit, test_constant_hi)
{
    // trace->open("test_constant_hi.vcd");

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

TEST(uart_first_bit, test_constant_hi_disable)
{
    // trace->open("test_constant_hi_disable.vcd");

    uint8_t output[2*OSR] = {0};

    tb->i_rx = 1;

    for (int i = 0; i < OSR; i++)
    {
        output[i] = tb->o_found;
        tick();
    }

    tb->i_en = 0;

    for (int i = OSR; i < 2*OSR; i++)
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

    for (int i = (OSR/2) + 1 + PULSE_WIDTH; i < 2*OSR; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, output[i], "Bit Low");
    }

}

void rx_hi_and_run(uint8_t count, uint8_t * output, uint8_t length)
{
    for (int i = 0; i < count; i ++)
    {
        tb->i_rx = 1;
        output[i] = tb->o_found;
        tick();
    }

    for (int i = count; i < length; i++)
    {
        tb->i_rx = 0;
        output[i] = tb->o_found;
        tick();
    }
}

TEST(uart_first_bit, test_pulse_below_osr_div_2)
{
    // trace->open("test_pulse_below_osr_div_2.vcd");

    uint8_t output[OSR+2] = {0};

    for (int i = 0; i < OSR/2; i++)
    {
        rx_hi_and_run(i, output, OSR+2);
        for (int j = 0; j < OSR+2; j++)
        {
            TEST_ASSERT_EQUAL(0, output[j]);
        }
    }
}

TEST(uart_first_bit, test_glitch_then_high)
{
    trace->open("test_glitch_then_high.vcd");
    uint8_t output[2*OSR] = {0};

    tb->i_rx = 1;
    output[0] = tb->o_found;
    tick();

    tb->i_rx = 0;
    output[1] = tb->o_found;
    tick();

    tb->i_rx = 1;
    for (int i = 2; i < 2*OSR; i++)
    {
        output[i] = tb->o_found;
        tick();
    }

    for (int i = 0; i <= (OSR/2)+2; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, output[i], "Debounce");
    }

    for (int i = (OSR/2) + 3; i < (OSR/2) + 3 + PULSE_WIDTH; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(1, output[i], "Bit HI");
    }

    // Back to low (will go high again in a few clocks, so we won't check that)
    for (int i = (OSR/2) + 3 + PULSE_WIDTH; i < (OSR/2) + 3 + PULSE_WIDTH + 3; i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(0, output[i], "Bit Low");
    }
}

// EOF
