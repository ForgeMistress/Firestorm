#include "stdafx.h"

#include <libApp/libApp.h>
#include <libCore/libCore.h>
#include <libExistence/libExistence.h>
#include <libHarnessed/libHarnessed.h>
#include <libIO/libIO.h>
#include <libMath/libMath.h>
#include <libMirror/libMirror.h>
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
RefPtr<TestHarness> libMirrorPrepareHarness(int ac, char** av);
RefPtr<TestHarness> libScriptPrepareHarness(int ac, char** av);

int main(int ac, char** av)
{

    Library<::Firestorm::libApp>::Initialize(ac,av);
    Library<::Firestorm::libCore>::Initialize(ac,av);
    Library<::Firestorm::libExistence>::Initialize(ac,av);
    Library<::Firestorm::libHarnessed>::Initialize(ac,av);
    Library<::Firestorm::libIO>::Initialize(ac,av);
    Library<::Firestorm::libMath>::Initialize(ac,av);
    Library<::Firestorm::libMirror>::Initialize(ac,av);
    Library<::Firestorm::libScene>::Initialize(ac,av);
    Library<::Firestorm::libScript>::Initialize(ac,av);
    Library<::Firestorm::libSerial>::Initialize(ac,av);
    Library<::Firestorm::libUI>::Initialize(ac,av);

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
    
    FIRE_LOG_DEBUG("");
	FIRE_LOG_DEBUG("======= LEAK CHECK =======");
    libCore::ReportMemoryLeaks();
	FIRE_LOG_DEBUG("===== END LEAK CHECK =====");
	FIRE_LOG_DEBUG("");

    FIRE_LOG_DEBUG("Press 'return' to close...");
    std::cin.get();

    return overallFailureCount;
}
