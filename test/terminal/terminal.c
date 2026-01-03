
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vterminal.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vterminal * tb;
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

TEST_GROUP(terminal);

uint32_t added = 0;

TEST_SETUP(terminal) 
{
    tb = new Vterminal;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(terminal)
{
    delete tb;
}

TEST_GROUP_RUNNER(terminal)
{
    RUN_TEST_CASE(terminal, test_space);
    RUN_TEST_CASE(terminal, test_bs);
    RUN_TEST_CASE(terminal, test_newline);
}

enum {
    STATE_IDLE = 0,
    STATE_SEND_LAST,
    STATE_SEND_BACKSPACE_SPACE_BACKSPACE,
    STATE_SEND_SPACE_BACKSPACE,
    STATE_SEND_BACKSPACE,
    STATE_SEND_NEWLINE,
    STATE_WAIT_FOR_TX
};

void rx_send(char c)
{
    tb->i_rx_data = c;
    tb->i_rx_ready = 1;

    tick();

    tb->i_rx_ready = 0;
}

void tx_pump(void)
{
    tb->i_tx_next = 1;
    tick();
    tb->i_tx_next = 0;
    tick();
}

TEST(terminal, test_space)
{
    trace->open("test_space.vcd");

    tb->i_en = 1;
    rx_send(' ');

    TEST_ASSERT_EQUAL(tb->d_index, 1);
    TEST_ASSERT_EQUAL(tb->o_tx_data, ' ');
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 1);
    TEST_ASSERT_EQUAL(tb->d_state, STATE_WAIT_FOR_TX);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);

    tx_pump();

    TEST_ASSERT_EQUAL(tb->d_index, 1);
    TEST_ASSERT_EQUAL(tb->o_tx_data, ' ');
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 0);
    TEST_ASSERT_EQUAL(tb->d_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);
}

TEST(terminal, test_bs)
{
    trace->open("test_bs.vcd");

    tb->i_en = 1;
    rx_send('?');
    tx_pump();
 
    rx_send(0x08); // Backspace

    TEST_ASSERT_EQUAL(tb->d_state, STATE_WAIT_FOR_TX);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_SEND_SPACE_BACKSPACE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_data, 0x08);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 1);

    tx_pump(); 

    TEST_ASSERT_EQUAL(tb->d_state, STATE_SEND_SPACE_BACKSPACE);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 0);
    TEST_ASSERT_EQUAL(tb->o_tx_data, 0x08);

    tick();

    TEST_ASSERT_EQUAL(tb->d_state, STATE_WAIT_FOR_TX);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_SEND_BACKSPACE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 1);
    TEST_ASSERT_EQUAL(tb->o_tx_data, ' ');

    tx_pump();

    TEST_ASSERT_EQUAL(tb->d_state, STATE_SEND_BACKSPACE);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 0);
    TEST_ASSERT_EQUAL(tb->o_tx_data, ' ');

    tick();

    TEST_ASSERT_EQUAL(tb->d_state, STATE_WAIT_FOR_TX);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 1);
    TEST_ASSERT_EQUAL(tb->o_tx_data, 0x08);

    tx_pump();
    
    TEST_ASSERT_EQUAL(tb->d_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);
    TEST_ASSERT_EQUAL(0, tb->d_line);
    TEST_ASSERT_EQUAL(0, tb->d_index);
    TEST_ASSERT_EQUAL(tb->o_tx_ready, 0);
    TEST_ASSERT_EQUAL(tb->o_tx_data, 0x08);
}

TEST(terminal, test_newline)
{
    trace->open("test_newline.vcd");

    tb->i_en = 1;

    rx_send('T');
    tx_pump();
    rx_send('E');
    tx_pump();
    rx_send('S');
    tx_pump();
    rx_send('T');
    tx_pump();
 
    rx_send('\n'); // newline

    TEST_ASSERT_EQUAL(tb->d_state, STATE_WAIT_FOR_TX);
    TEST_ASSERT_EQUAL(tb->d_next_state, STATE_IDLE);

    TEST_ASSERT_EQUAL(tb->o_len, 5);
    TEST_ASSERT_EQUAL(tb->d_line, 1);
    TEST_ASSERT_EQUAL(tb->d_index, 0);

    TEST_ASSERT_EQUAL('\n', tb->o_tx_data);
    TEST_ASSERT_EQUAL(1, tb->o_tx_ready);

    tx_pump(); 
}

// EOF
