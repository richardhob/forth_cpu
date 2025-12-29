
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vword_to_int.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#define START     (1u)
#define DATA      (8u)
#define STOP      (2u)
#define COOLDOWN  (1u)

#define OSR      (16u)

Vword_to_int * tb;
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

TEST_GROUP(word_to_int);

uint32_t added = 0;

TEST_SETUP(word_to_int) 
{
    tb = new Vword_to_int;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }
}

TEST_TEAR_DOWN(word_to_int)
{
    delete tb;
}

TEST_GROUP_RUNNER(word_to_int)
{
    RUN_TEST_CASE(word_to_int, test_simple);
    RUN_TEST_CASE(word_to_int, test_max);
    RUN_TEST_CASE(word_to_int, test_zero);
    RUN_TEST_CASE(word_to_int, test_empty);
    RUN_TEST_CASE(word_to_int, test_invalid);
}

TEST(word_to_int, test_simple)
{
    trace->open("test_simple.vcd");

    tb->i_en = 1;

    tb->i_word[0] = '1';
    tb->i_word[1] = '0';
    tb->i_word[2] = '0';

    tb->i_len = 3;

    tick();

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(100, tb->o_data);
}

TEST(word_to_int, test_max)
{
    tb->i_en = 1;

    uint32_t expected = 0;;
    for (int i = 0; i < 32; i++)
    {
        expected = expected * 10 + 9;
        tb->i_word[i] = '9';
    }

    tb->i_len = 32;

    tick();

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(expected, tb->o_data);
}

TEST(word_to_int, test_zero)
{
    tb->i_en = 1;
    tb->i_word[0] = '0';
    tb->i_len = 1;

    tick();

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL(0, tb->o_data);
}

TEST(word_to_int, test_empty)
{
    tb->i_en = 1;
    tb->i_word[0] = ' ';
    tb->i_len = 0;

    tick();

    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(word_to_int, test_invalid)
{
    tb->i_en = 1;
    tb->i_word[0] = 'T';
    tb->i_word[1] = 'e';
    tb->i_word[2] = 's';
    tb->i_word[3] = 't';
    tb->i_len = 4;

    tick();

    TEST_ASSERT_EQUAL(1, tb->o_err);
}

// EOF
