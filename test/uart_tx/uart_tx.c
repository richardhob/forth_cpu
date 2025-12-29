
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vuart_tx.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define OSR      (16u)
#define START    (1u)
#define DATA     (8u)
#define STOP     (2u)
#define COOLDOWN (1u)
#define BITS     (START + DATA + STOP)

typedef enum {
    STATE_RESET = 0,
    STATE_IDLE,
    STATE_START,
    STATE_DATA,
    STATE_STOP,
    STATE_COOLDOWN
} tx_state;

Vuart_tx * tb;
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

TEST_GROUP(uart_tx);

uint32_t added = 0;

TEST_SETUP(uart_tx) 
{
    tb = new Vuart_tx;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(uart_tx)
{
    delete tb;
}

TEST_GROUP_RUNNER(uart_tx)
{
    RUN_TEST_CASE(uart_tx, test_transmit_single);
    RUN_TEST_CASE(uart_tx, test_transmit_all);
}

void transmit(uint8_t data)
{
    tb->i_en = 1;
    tick();

    TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected initial d_state to be IDLE (0)");
    TEST_ASSERT_EQUAL_MESSAGE(1, tb->o_next, "Expected initial o_next to be 1");

    tb->i_data = data;
    tb->i_ready = 1;

    tick();

    tb->i_ready = 0;
    TEST_ASSERT_EQUAL_MESSAGE(data, tb->d_data, "Expected data and d_data to match");

    for (int i = 0; i < (START * OSR); i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(STATE_START, tb->d_state, "Expected d_state to be START (1)");
        tick();
    }

    char buffer[256];
    uint8_t data_bit;
    uint8_t expected;
    for (int i = 0; i < (DATA * OSR); i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(STATE_DATA, tb->d_state, "Expected d_state to be DATA (2)");

        // LSB First in DATA BITS
        data_bit = i >> 4; // log2(OSR);

        expected = (data >> data_bit) & 0x01;
        snprintf(buffer, 256, "Expected o_tx to be %d, was %d (data %d, bit %d, i %d)\r\n", expected, tb->o_tx, tb->d_data, data_bit, i);
        TEST_ASSERT_EQUAL_MESSAGE(expected, tb->o_tx, buffer);
        // printf(buffer);

        tick();
    }

    for (int i = 0; i < (STOP * OSR); i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(STATE_STOP, tb->d_state, "Expected d_state to be STOP (3)");
        tick();
    }

    for (int i = 0; i < (COOLDOWN * OSR); i++)
    {
        TEST_ASSERT_EQUAL_MESSAGE(STATE_COOLDOWN, tb->d_state, "Expected d_state to be COOLDOWN (4)");
        tick();
    }

    TEST_ASSERT_EQUAL_MESSAGE(STATE_IDLE, tb->d_state, "Expected d_state to be IDLE (0)");
}

TEST(uart_tx, test_transmit_single)
{
    trace->open("test_transmit.vcd");
    transmit(0x0);
}

TEST(uart_tx, test_transmit_all)
{
    for (int i = 0; i < 255; i++)
    {
        transmit(i);
    }
}

// EOF
