#include "stdafx.h"

#include <libCore/libCore.h>
#include <libExistence/libExistence.h>
#include <libHarnessed/libHarnessed.h>
#include <libIO/libIO.h>
#include <libMirror/libMirror.h>
#include <libScript/libScript.h>

#include <libCore/RefPtr.h>
#include <libIO/Logger.h>
#include <libHarnessed/libHarnessed.h>

using namespace Firestorm;

RefPtr<TestHarness> libCorePrepareHarness(int ac, char** av);
RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av);
RefPtr<TestHarness> libHarnessedPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libIOPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libMirrorPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libScriptPrepareHarness(int ac, char** av);

int main(int ac, char** av)
{
    libCore::Initialize(ac,av);
    libExistence::Initialize(ac,av);
    libHarnessed::Initialize(ac,av);
    libIO::Initialize(ac,av);
    libMirror::Initialize(ac,av);
    libScript::Initialize(ac,av);

    Vector<RefPtr<TestHarness>> TESTS = {
        libCorePrepareHarness(ac, av),
        libExistencePrepareHarness(ac, av),
        libHarnessedPrepareHarness(ac, av),
        libIOPrepareHarness(ac, av),
        libMirrorPrepareHarness(ac, av),
        libScriptPrepareHarness(ac, av)
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
