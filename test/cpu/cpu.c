
// Tests to write
//
// 1. Error checks for APIs

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vcpu.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

#include "operations.h"

Vcpu * tb;
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

TEST_GROUP(cpu);

uint32_t added = 0;

TEST_SETUP(cpu) 
{
    tb = new Vcpu;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }

    tb->i_rst = 1;
    tick();
    tb->i_rst = 0;
}

TEST_TEAR_DOWN(cpu)
{
    delete tb;
}

TEST_GROUP_RUNNER(cpu)
{
    RUN_TEST_CASE(cpu, test_push);
    RUN_TEST_CASE(cpu, test_add);
    RUN_TEST_CASE(cpu, test_sub);
    RUN_TEST_CASE(cpu, test_mul);
    RUN_TEST_CASE(cpu, test_div);
    RUN_TEST_CASE(cpu, test_div_0);
    RUN_TEST_CASE(cpu, test_dup);
    RUN_TEST_CASE(cpu, test_rot);
    RUN_TEST_CASE(cpu, test_swap);
}

TEST(cpu, test_push)
{
    trace->open("test_push.vcd");

    tb->i_en = 1;
    for (int i = 0; i < 10; i++)
    {
        op_push(i);

        TEST_ASSERT_EQUAL(i+1, tb->d_index);
        TEST_ASSERT_EQUAL(i, tb->d_stack[i]);
    }
}

TEST(cpu, test_add)
{
    trace->open("test_add.vcd");

    tb->i_en = 1;
    op_push(1);
    op_push(2);
    op_add();

    TEST_ASSERT_EQUAL(1, tb->d_index);
    TEST_ASSERT_EQUAL(3, tb->d_stack[0]);
}

TEST(cpu, test_sub)
{
    trace->open("test_sub.vcd");

    tb->i_en = 1;
    op_push(3);
    op_push(2);
    op_sub();

    TEST_ASSERT_EQUAL(1, tb->d_index);
    TEST_ASSERT_EQUAL(1, tb->d_stack[0]);
}

TEST(cpu, test_mul)
{
    trace->open("test_mul.vcd");

    tb->i_en = 1;
    op_push(3);
    op_push(3);
    op_mul();

    TEST_ASSERT_EQUAL(1, tb->d_index);
    TEST_ASSERT_EQUAL(9, tb->d_stack[0]);
}

TEST(cpu, test_div)
{
    trace->open("test_div.vcd");

    tb->i_en = 1;
    op_push(9);
    op_push(3);
    op_div();

    tick();

    TEST_ASSERT_EQUAL(1, tb->d_index);
    TEST_ASSERT_EQUAL(3, tb->d_stack[0]);
}

TEST(cpu, test_div_0)
{
    trace->open("test_div_0.vcd");

    tb->i_en = 1;
    op_push(9);
    op_push(0);
    op_div();

    TEST_ASSERT_EQUAL(ERROR_DIVIDE_BY_ZERO, tb->o_err);
}

TEST(cpu, test_dup)
{
    trace->open("test_dup.vcd");

    tb->i_en = 1;
    op_push(0xFF);
    op_dup();

    TEST_ASSERT_EQUAL(2, tb->d_index);
    TEST_ASSERT_EQUAL_HEX(0xFF, tb->d_stack[0]);
    TEST_ASSERT_EQUAL_HEX(0xFF, tb->d_stack[1]);
}

TEST(cpu, test_rot)
{
    trace->open("test_rot.vcd");

    tb->i_en = 1;
    op_push(3);
    op_push(2);
    op_push(1);
    op_rot();

    TEST_ASSERT_EQUAL(3, tb->d_index);
    TEST_ASSERT_EQUAL_HEX(0x01, tb->d_stack[0]);
    TEST_ASSERT_EQUAL_HEX(0x03, tb->d_stack[1]);
    TEST_ASSERT_EQUAL_HEX(0x02, tb->d_stack[2]);
}

TEST(cpu, test_swap)
{
    trace->open("test_swap.vcd");

    tb->i_en = 1;
    op_push(2);
    op_push(1);
    op_swap();

    TEST_ASSERT_EQUAL(2, tb->d_index);
    TEST_ASSERT_EQUAL_HEX(0x01, tb->d_stack[0]);
    TEST_ASSERT_EQUAL_HEX(0x02, tb->d_stack[1]);
}

// EOF
