
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_rx_to_tx.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define START     (1u)
#define DATA      (8u)
#define STOP      (2u)
#define COOLDOWN  (1u)

#define OSR      (16u)

Vuart_rx_to_tx * tb;
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

TEST_GROUP(uart_rx_to_tx);

uint32_t added = 0;

TEST_SETUP(uart_rx_to_tx) 
{
    tb = new Vuart_rx_to_tx;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(uart_rx_to_tx)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_rx_to_tx)
{
    RUN_TEST_CASE(uart_rx_to_tx, test_message);
    RUN_TEST_CASE(uart_rx_to_tx, test_single);
}

void receive(uint8_t data)
{
    tb->i_en = 1;
    tb->i_rx = 1; // RX is active LOW

    tick();

    tb->i_rx = 0;
    for (uint32_t i = 0; i < (START*OSR); i++) tick();

    uint8_t value;
    for (uint32_t i = 0; i < DATA; i++)
    {
        // We can't use ~ here ... Verilator gets upset when we do that?
        value = !(data & 0x01);
        tb->i_rx = value;

        data = data >> 1;
        for (uint32_t j = 0; j < OSR; j++) tick();
    }

    // At this point the RX Ready line should be good. We'll set the i_rx to the
    // default state to prevent dummy messages.
    //
    // The STOP and COOLDOWN parts don't really care what the state of the RX
    // line is.
    tb->i_rx = 1;
}

void transmit(uint8_t * data)
{
    // TX Should start as soon as it receives the Ready signal from the RX side.
    for (uint32_t i = 0; i < (START * OSR); i++) tick();

    for (uint32_t i = 0; i < DATA; i++)
    {
        for (uint32_t j = 0; j < OSR; j++)
        {
            if (j == (OSR/2)) *data = (*data & (~(1 << i))) | ((~tb->o_tx) << i);
            tick();
        }
    }

    for (uint32_t i = 0; i < (STOP * OSR); i++) tick();
    for (uint32_t i = 0; i < (COOLDOWN * OSR); i++) tick();
}

TEST(uart_rx_to_tx, test_single)
{
    // trace->open("test_single.vcd");

    uint8_t expected = 0xAA;
    uint8_t actual   = 0x00;

    receive(expected);
    transmit(&actual);

    TEST_ASSERT_EQUAL(expected, actual);
}

TEST(uart_rx_to_tx, test_message)
{
    trace->open("test_message.vcd");
    unsigned char expected[] = "Hello, World";
    unsigned char actual[256] = {0};

    uint32_t length = sizeof(expected)/sizeof(expected[0]);

    for (int i = 0; i < length; i++)
    {
        receive(expected[i]);
        transmit((uint8_t *)&(actual[i]));
        TEST_ASSERT_EQUAL(expected[i], actual[i]);
    }
}

// EOF
