
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_rx.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define OSR      (16u)

#define START    (1u)
#define DATA     (8u)
#define STOP     (2u)
#define COOLDOWN (1u)

typedef enum {
    STATE_RESET = 0,
    STATE_IDLE,
    STATE_START_DEBOUNCE,
    STATE_START_VALID,
    STATE_START,
    STATE_DATA_D0,
    STATE_DATA_D1,
    STATE_DATA_D2,
    STATE_DATA_D3,
    STATE_DATA_D4,
    STATE_DATA_D5,
    STATE_DATA_D6,
    STATE_DATA_D7,
    STATE_DATA_END,
    STATE_STOP
} rx_state;

Vuart_rx * tb;
extern VerilatedVcdC * trace;

void tick()
{
    static uint32_t g_tick = 1;
    tb->eval();

    // 2ns Before the tick
    if (trace) trace->dump(g_tick * 10 - 2);

    tb->i_divided_clk = 1;
    tb->eval();

    // 10ns Tick 
    if (trace) trace->dump(g_tick * 10);
    
    tb->i_divided_clk = 0;
    tb->eval();

    // Trailing Edge
    if (trace) 
    {
        trace->dump(g_tick * 10 + 5);
        trace->flush();
    }

    g_tick = g_tick + 1;
}

TEST_GROUP(uart_rx);

uint32_t added = 0;

TEST_SETUP(uart_rx) 
{
    tb = new Vuart_rx;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(uart_rx)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_rx)
{
    RUN_TEST_CASE(uart_rx, test_ignore_pulse);
    RUN_TEST_CASE(uart_rx, test_receive_single);
    RUN_TEST_CASE(uart_rx, test_receive_all);
}

void receive(uint8_t data_in)
{
    // State should be IDLE
    tb->i_en = 1;
    tick();

    TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected state to be IDLE");

    // Start Bit
    tb->i_rx = 1;
    for (uint32_t i = 0; i < (START * OSR - 1); i++)
    {
        tick();

        if (i < ((START * OSR) / 4))
        {
            TEST_ASSERT_EQUAL_MESSAGE(STATE_START_DEBOUNCE, tb->d_state, "Expected state to be START Debounce");
        }
        else if (i <= ((START * OSR) / 2))
        {
            TEST_ASSERT_EQUAL_MESSAGE(STATE_START_VALID, tb->d_state, "Expected state to be START Valid");
        }
        else // (i < (START * OSR)
        {
            TEST_ASSERT_EQUAL_MESSAGE(STATE_START, tb->d_state, "Expected state to be START");
        }
    }

    tick();
    TEST_ASSERT_EQUAL_MESSAGE(STATE_DATA_D0, tb->d_state, "Expected state to be D0");

    // Data Bits
    uint32_t temp;
    for (uint32_t i = 0; i < DATA; i++)
    {
        temp = (data_in >> i) & 0x01;
        tb->i_rx = temp;

        for (uint32_t j = 0; j < OSR; j++)
        {
            tick();
        }
    }

    tick();
    TEST_ASSERT_EQUAL_MESSAGE(STATE_STOP, tb->d_state, "Expected state to be STOP");

    // Stop Bits 
    tb->i_rx = 1;
    for (uint32_t i = 0; i < (STOP * OSR) - 1; i++)
    {
        tick();
        TEST_ASSERT_EQUAL_MESSAGE(STATE_STOP, tb->d_state, "Expected state to be STOP");
    }
    tb->i_rx = 0;

    tick();
    TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected FINAL state to be IDLE");

    // Done
}

TEST(uart_rx, test_receive_single)
{
    // trace->open("test_receive.vcd");
    receive(0xAA);
    TEST_ASSERT_EQUAL_MESSAGE(0xAA, tb->o_data, "Expected data to match (0xAA)");
}

TEST(uart_rx, test_receive_all)
{
    for (int i = 0; i < 0xFF; i++)
    {
        receive(i);

        for (int i = 0; i < 10; i ++) tick();

        TEST_ASSERT_EQUAL_MESSAGE(i, tb->o_data, "Expected data to match");
    }
}

void pulse(uint8_t width)
{
    // State should be IDLE
    tb->i_en = 1;
    tick();

    TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected state to be IDLE");

    // Start Bit pulse
    tb->i_rx = 1;
    for (uint32_t i = 0; i < width; i++) tick();
    tb->i_rx = 0;

    // The start bit has a debouncer - which means that we have to make more the
    // 4 clocks to get to the bit width bits
    for (uint32_t i = 0; i < ((START * OSR)/2) - width; i++) tick();
}

TEST(uart_rx, test_ignore_pulse)
{
    trace->open("test_ignore_pulse.vcd");
    for (int i = 0; i < 8; i++)
    {
        pulse(i);
        TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected state to return to IDLE with short pulse");
    }
}

// EOF
