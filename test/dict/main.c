
#include "unity.h"
#include "unity_fixture.h"

#include "Vdict.h"

#include "verilated.h"
#include "verilated_vcd_c.h"

VerilatedVcdC * trace;

void run_all_tests(void)
{
    RUN_TEST_GROUP(dict);
}

extern Vdict * tb;

int main(int argc, const char ** argv)
{
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    trace = new VerilatedVcdC;

    tb = new Vdict;
    tb->trace(trace, 99);

    trace->open("dict.vcd");

    UnityMain(argc, argv, run_all_tests);

    return 0;
}

// EOF
