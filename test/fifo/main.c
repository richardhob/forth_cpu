
#include "unity.h"
#include "unity_fixture.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

void run_all_tests(void)
{
    RUN_TEST_GROUP(fifo);
}

int main(int argc, const char ** argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    UnityMain(argc, argv, run_all_tests);
    return 0;
}

// EOF
