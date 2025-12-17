
#include "unity.h"
#include "unity_fixture.h"

#include "Vfind_index.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedVcdC * trace;

void run_all_tests(void)
{
    RUN_TEST_GROUP(find_index);
}

extern Vfind_index * tb;

int main(int argc, const char ** argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    trace = new VerilatedVcdC;

    tb = new Vfind_index;
    tb->trace(trace, 99);

    trace->open("find_index.vcd");

    UnityMain(argc, argv, run_all_tests);

    return 0;
}

// EOF
