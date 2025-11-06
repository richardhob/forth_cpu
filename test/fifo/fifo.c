
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(fifo);

TEST_SETUP(fifo) 
{
}

TEST_TEAR_DOWN(fifo)
{
}

TEST_GROUP_RUNNER(fifo)
{
    RUN_TEST_CASE(fifo, test_fail);
}

TEST(fifo, test_fail)
{
    TEST_FAIL_MESSAGE("TODO: Fix");
}

// EOF
