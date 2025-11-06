
#include "unity.h"
#include "unity_fixture.h"

void run_all_tests(void)
{
    RUN_TEST_GROUP(fifo);
}

int main(int argc, const char * argv[])
{
    UnityMain(argc, argv, run_all_tests);
    return 0;
}

// EOF
