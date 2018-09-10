------------------------------------------------------------------------------------------------------------------------
--
--  UnitTestMainTemplate.lua
--
--  Contains the source code for the main entry point of the unit tester.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
UNIT_TEST_MAIN = [[
#include "stdafx.h"

{INCLUDES}

#include <libCore/RefPtr.h>
#include <libIO/Logger.h>
#include <libHarnessed/libHarnessed.h>

using namespace Firestorm;

{FWD_DECLARES}

int main(int ac, char** av)
{
{LIB_INITIALIZATIONS}

    Vector<RefPtr<TestHarness>> TESTS = {
{TEST_FUNCTIONS}
    };

    uint32_t overallFailureCount = 0;

    for(RefPtr<TestHarness>& harness : TESTS)
    {
        uint32_t testResults = harness->Run();
        overallFailureCount += testResults;

        // report the results for this particular test.
        FIRE_LOG_DEBUG("");
        FIRE_LOG_DEBUG("Completed", harness->GetName(), "with", testResults, "errors...");
        FIRE_LOG_DEBUG("-----------------------------------------------------------------------------------------------------------");
    }

    if(overallFailureCount > 0)
    {
        FIRE_LOG_DEBUG("Completed all tests with", overallFailureCount, "errors.");
    }
    else
    {
        FIRE_LOG_DEBUG("Completed all tests with no failures!");
    }

    FIRE_LOG_DEBUG("Press 'return' to close...");
    std::cin.get();

    return overallFailureCount;
}
]]
