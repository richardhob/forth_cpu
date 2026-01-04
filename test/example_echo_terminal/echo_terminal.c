
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vecho_terminal.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#include "uart.h"

#define START     (1u)
#define DATA      (8u)
#define STOP      (2u)
#define COOLDOWN  (1u)

#define OSR      (16u)

Vecho_terminal * tb;
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

TEST_GROUP(echo_terminal);

uint32_t added = 0;

TEST_SETUP(echo_terminal) 
{
    tb = new Vecho_terminal;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst    = 1;

    tick(); // 1 clock of Reset

    tb->i_rst    = 0;
}

TEST_TEAR_DOWN(echo_terminal)
{
    delete tb;
}

TEST_GROUP_RUNNER(echo_terminal)
{
    RUN_TEST_CASE(echo_terminal, test_message);
    RUN_TEST_CASE(echo_terminal, test_single);
}

TEST(echo_terminal, test_single)
{
    trace->open("test_single.vcd");

    uint8_t expected = 0xAA;
    uint8_t actual   = 0x00;

    tb->i_en = 1;
    receive(expected);
    transmit(&actual);

    TEST_ASSERT_EQUAL(expected, actual);
}

TEST(echo_terminal, test_message)
{
    trace->open("test_message.vcd");

    unsigned char expected[] = "Hello, World";
    unsigned char actual[256] = {0};

    uint32_t length = sizeof(expected)/sizeof(expected[0]);

    tb->i_en = 1;
    for (int i = 0; i < length; i++)
    {
        receive(expected[i]);
        transmit((uint8_t *)&(actual[i]));
        TEST_ASSERT_EQUAL(expected[i], actual[i]);
    }
}

// EOF
