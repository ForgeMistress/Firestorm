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

	uint32_t errors = 0;
	TestCase testCase;
	if(!m_quietly)
	{
		cout << "Test: " << m_name << endl << endl;
	}

	for(size_t i=0;i<m_cases.size(); ++i)
	{
		const TestFunction_t& test = m_cases[i];
		const String& testName = m_caseNames[i];

		String testResult;
		String errorStr;

		bool passed = false;
		try
		{
			test(testCase);
			testResult = "[PASSED]";
			passed = true;
		}
		catch(const TestCase::AssertionException& ex)
		{
			testResult = "[FAILED]";
			errorStr = String("        Error: ") + ex.GetMessage();
			++errors;
		}
		catch(std::exception& e)
		{
			testResult = "[FAILED]";
			errorStr = String("        Error: ") + e.what();
			++errors;
		}

		if(!m_quietly)
		{
			cout << testResult << " " << testName << endl;

			if(!passed)
			{
				cout << errorStr << endl;
			}
		}
	}
	return errors;
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

CLOSE_NAMESPACE(Firestorm);