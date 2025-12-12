
#include "unity.h"
#include "unity_fixture.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedVcdC * trace;

void run_all_tests(void)
{
    RUN_TEST_GROUP(tokenizer);
}

int main(int argc, const char ** argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    trace = new VerilatedVcdC;

    UnityMain(argc, argv, run_all_tests);

    return 0;
}

// EOF
