#include "stdafx.h"

#include <libApp/libApp.h>
#include <libCore/libCore.h>
#include <libExistence/libExistence.h>
#include <libHarnessed/libHarnessed.h>
#include <libIO/libIO.h>
#include <libMath/libMath.h>
#include <libScene/libScene.h>
#include <libScript/libScript.h>
#include <libSerial/libSerial.h>
#include <libUI/libUI.h>

#include <libCore/RefPtr.h>
#include <libCore/LibraryRegistrar.h>
#include <libCore/Logger.h>

using namespace Firestorm;

RefPtr<TestHarness> libCorePrepareHarness(int ac, char** av);
RefPtr<TestHarness> libExistencePrepareHarness(int ac, char** av);
RefPtr<TestHarness> libHarnessedPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libIOPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libScriptPrepareHarness(int ac, char** av);

int main(int ac, char** av)
{
    size_t overallFailureCount = 0;
    Library<::Firestorm::libApp>::Initialize(ac,av);
    Library<::Firestorm::libCore>::Initialize(ac,av);
    Library<::Firestorm::libExistence>::Initialize(ac,av);
    Library<::Firestorm::libHarnessed>::Initialize(ac,av);
    Library<::Firestorm::libIO>::Initialize(ac,av);
    Library<::Firestorm::libMath>::Initialize(ac,av);
    Library<::Firestorm::libScene>::Initialize(ac,av);
    Library<::Firestorm::libScript>::Initialize(ac,av);
    Library<::Firestorm::libSerial>::Initialize(ac,av);
    Library<::Firestorm::libUI>::Initialize(ac,av);

    vector<RefPtr<TestHarness>> TESTS = {
        libCorePrepareHarness(ac, av),
        libExistencePrepareHarness(ac, av),
        libHarnessedPrepareHarness(ac, av),
        libIOPrepareHarness(ac, av),
        libScriptPrepareHarness(ac, av)
    };


    try
    {
        for(RefPtr<TestHarness>& harness : TESTS)
        {
            size_t testResults = harness->Run();
            overallFailureCount += testResults;

            // report the results for this particular test.
            FIRE_LOG_DEBUG("");
            FIRE_LOG_DEBUG("Completed %s with %d errors...", harness->GetName(), testResults);
            FIRE_LOG_DEBUG("-----------------------------------------------------------------------------------------------------------");
        }

        if(overallFailureCount > 0)
        {
            FIRE_LOG_DEBUG("Completed all tests with %d errors.", overallFailureCount);
        }
        else
        {
            FIRE_LOG_DEBUG("Completed all tests with no failures!");
        }
        
        libCore::ReportMemoryLeaks();

        FIRE_LOG_DEBUG("Press 'return' to close...");
        std::cin.get();
    }
    catch(AssertionException& e)
    {
        FIRE_LOG_ERROR("!!!Uncaught exception!!! %s", e.Report());
    }
    TESTS.clear();

    return overallFailureCount;
}