#if !defined SIGNAL_MANAGER_TEST
#define SIGNAL_MANAGER_TEST

#include "Test.hpp"
#include "TestCases.hpp"
#include "TestMacros.hpp"

TEST_CASES_BEGIN(SignalManager_test)

    TEST(Signal)
    TEST(IsSignalDelivered)
    TEST(GenerateAllSignals)

TEST_CASES_END(SignalManager_test)

#define GET_SIGNAL_MANAGER(signal_manager)      \
    try                                         \
    {                                           \
        signal_manager = new SignalManager();   \
    }                                           \
    catch (std::runtime_error&)                 \
    {                                           \
        return Test::SKIPPED;                   \
    }

#endif
