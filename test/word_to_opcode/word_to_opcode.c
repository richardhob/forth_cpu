
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "unity.h"
#include "unity_fixture.h"

#include "Vword_to_opcode.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

Vword_to_opcode * tb;
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

TEST_GROUP(word_to_opcode);

uint32_t added = 0;

TEST_SETUP(word_to_opcode) 
{
    tb = new Vword_to_opcode;

    if (added == 0)
    {
        tb->trace(trace, 99);
        added = 1;
    }
}

TEST_TEAR_DOWN(word_to_opcode)
{
    delete tb;
}

TEST_GROUP_RUNNER(word_to_opcode)
{
    RUN_TEST_CASE(word_to_opcode, test_valid);
    RUN_TEST_CASE(word_to_opcode, test_simple);
    RUN_TEST_CASE(word_to_opcode, test_empty);
    RUN_TEST_CASE(word_to_opcode, test_invalid);
}

enum {
    OPCODE_IDLE = 0,
    OPCODE_PUSH,
    OPCODE_POP,
    OPCODE_ADD,
    OPCODE_SUBTRACT,
    OPCODE_MULTIPLY,
    OPCODE_DIVIDE,
    OPCODE_DUP,
    OPCODE_ROT,
    OPCODE_SWAP
};

char opcode_strings[][256] = {
    ".",
    "+",
    "-",
    "*",
    "/",
    "DUP",
    "ROT",
    "SWAP"
};

uint8_t opcode_values[] = {
    OPCODE_POP,
    OPCODE_ADD,
    OPCODE_SUBTRACT,
    OPCODE_MULTIPLY,
    OPCODE_DIVIDE,
    OPCODE_DUP,
    OPCODE_ROT,
    OPCODE_SWAP
};

TEST(word_to_opcode, test_simple)
{
    trace->open("test_simple.vcd");

    tb->i_en = 1;

    tb->i_word[0] = '.';
    tb->i_len = 1;

    tick();

    TEST_ASSERT_EQUAL(0, tb->o_err);
    TEST_ASSERT_EQUAL_HEX(0, tb->o_data);
    TEST_ASSERT_EQUAL_HEX(OPCODE_POP, tb->o_opcode);
}

TEST(word_to_opcode, test_valid)
{
    trace->open("test_valid.vcd");

    uint16_t expected = 0;
    uint32_t num_opcodes = sizeof(opcode_strings) / sizeof(opcode_strings[0]);

    for (int i = 0; i < num_opcodes; i++)
    {
        expected = opcode_values[i];
        for (int j = 0; j < 256; j++)
        {
            if (opcode_strings[i][j] == '\0') break;
            else {
                tb->i_word[j] = opcode_strings[i][j];
                tb->i_len = j + 1;
            }
        }

        tb->i_en = 1;
        tick();

        TEST_ASSERT_EQUAL(0, tb->o_err);
        TEST_ASSERT_EQUAL_HEX(0, tb->o_data);
        TEST_ASSERT_EQUAL_HEX(expected, tb->o_opcode);
    }
}

TEST(word_to_opcode, test_empty)
{
    tb->i_en = 1;
    tb->i_word[0] = ' ';
    tb->i_len = 0;

    tick();

    TEST_ASSERT_EQUAL(1, tb->o_err);
}

TEST(word_to_opcode, test_invalid)
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
