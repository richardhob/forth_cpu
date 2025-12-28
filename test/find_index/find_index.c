
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vfind_index.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vfind_index * tb;
extern VerilatedVcdC * trace;

void tick()
{
    static uint32_t g_tick = 1;
    tb->eval();

    // 2ns Before the tick
    if (trace) trace->dump(g_tick * 10 - 2);

    tb->i_update = 1;
    tb->eval();

    // 10ns Tick 
    if (trace) trace->dump(g_tick * 10);
    
    tb->i_update = 0;
    tb->eval();

    // Trailing Edge
    if (trace) 
    {
        trace->dump(g_tick * 10 + 5);
        trace->flush();
    }

    g_tick = g_tick + 1;
}

TEST_GROUP(find_index);

TEST_SETUP(find_index) 
{
}

TEST_TEAR_DOWN(find_index)
{
}

TEST_GROUP_RUNNER(find_index)
{

    RUN_TEST_CASE(find_index, test_duplicate_lowest_index);
    RUN_TEST_CASE(find_index, test_first);
    RUN_TEST_CASE(find_index, test_second);

    RUN_TEST_CASE(find_index, test_first_all);
    RUN_TEST_CASE(find_index, test_second_all);
    RUN_TEST_CASE(find_index, test_not_present);
}

TEST(find_index, test_duplicate_lowest_index)
{
    tb->i_key[0] = 0x0A;
    tb->i_keys[0][0] = 0x0A;
    tb->i_keys[1][0] = 0x0A;

    tick();

    TEST_ASSERT_EQUAL(tb->o_found, 1);
    TEST_ASSERT_EQUAL(tb->o_index, 0);
}

TEST(find_index, test_first)
{
    tb->i_key[0] = 0x0A;
    tb->i_keys[0][0] = 0x0A;
    tb->i_keys[1][0] = 0x00;

    tick();

    TEST_ASSERT_EQUAL(tb->o_found, 1);
    TEST_ASSERT_EQUAL(tb->o_index, 0);
}

TEST(find_index, test_second)
{
    tb->i_key[0] = 0x0A;
    tb->i_keys[0][0] = 0x00;
    tb->i_keys[1][0] = 0x0A;

    tick();

    TEST_ASSERT_EQUAL(tb->o_found, 1);
    TEST_ASSERT_EQUAL(tb->o_index, 1);
}

TEST(find_index, test_first_all)
{
    for (uint32_t i = 0; i < 0xFF; i++)
    {
        tb->i_key[0] = 0xFF & i;
        tb->i_keys[0][0] = 0xFF & i;
        tb->i_keys[1][0] = 0xFF & ~i;

        tick();

        TEST_ASSERT_EQUAL(tb->o_found, 1);
        TEST_ASSERT_EQUAL(tb->o_index, 0);
    }
}

TEST(find_index, test_second_all)
{
    for (uint32_t i = 0; i < 0xFF; i++)
    {
        tb->i_key[0] = 0xFF & i;
        tb->i_keys[0][0] = 0xFF & ~i;
        tb->i_keys[1][0] = 0xFF & i;

        tick();

        TEST_ASSERT_EQUAL(tb->o_found, 1);
        TEST_ASSERT_EQUAL(tb->o_index, 1);
    }
}

TEST(find_index, test_not_present)
{
    for (uint32_t i = 0; i < 0xFF; i++)
    {
        tb->i_key[0] = 0xFF & i;
        tb->i_keys[0][0] = 0xFF & ~i;
        tb->i_keys[1][0] = 0xFF & ~i;

        tick();

        TEST_ASSERT_EQUAL(tb->o_found, 0);
        TEST_ASSERT_EQUAL(tb->o_index, 0);
    }
}

// EOF
