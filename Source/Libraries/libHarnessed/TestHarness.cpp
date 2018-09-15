///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestHarness.cpp
//
//  Abstract base class that holds a collection of TestCase instances.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TestHarness.h"
#include "TestCase.h"


OPEN_NAMESPACE(Firestorm);

TestHarness::TestHarness(const String& name, bool quietly)
: m_name(name)
, m_quietly(quietly)
{
}

uint32_t TestHarness::Run()
{
	FIRE_ASSERT(m_cases.size() == m_caseNames.size());

	Print("Test: %s", m_name);
	Print("");

	uint32_t finalErrorCount = 0;
	bool hasFailed = false;
	for(size_t i=0;i<m_cases.size(); ++i)
	{
		auto test = m_cases[i];
		const String& testName = m_caseNames[i];

		Print("Case[%d] %s",i, testName);
		TestCase testCase(testName, this);

		test(testCase);
		auto failures = testCase.m_failures;
		if(!failures.empty())
		{
			finalErrorCount += failures.size();
			Print("!!  Case [%d] failed with %d errors...", i, failures.size());
		}

		/*bool passed = false;
		try
		{
			test(testCase);
			testResult = "[PASSED]";
			passed = true;
		}
		catch(const TestCase::AssertionException& ex)
		{
			testResult = "[FAILED]";
			//errorStr = String("        Error: ") + ex.GetMessage();
			//++errors;
		}
		catch(std::exception& e)
		{
			testResult = "[FAILED]";
			//errorStr = String("        Error: ") + e.what();
			//++errors;
		}*/

		
		/*auto failures = testCase.GetFailures();
		errors += failures.size();
		if(failures.empty())
		{
			testResult = "[PASSED]";
		}
		if (!m_quietly)
		{
			cout << testResult << " " << testName << endl;
		}

		for(auto failure : failures)
		{
			if (!m_quietly)
			{
				cout << "        Error: " << failure << endl;
			}
		}*/
	}
	return finalErrorCount;
}

void TestHarness::It(const String& caseName, TestFunction_t testFunction)
{
	for(const auto& cn : m_caseNames)
	{
		FIRE_ASSERT(cn != caseName);
	}
	m_cases.push_back(testFunction);
	m_caseNames.push_back(caseName);
}

void TestHarness::ReportError(TestCase* tc, const String& message)
{
	Print("    Error: %s", message);
}

CLOSE_NAMESPACE(Firestorm);