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

OPEN_NAMESPACE(Elf);

TestHarness::TestHarness(const string& name, bool quietly)
: m_name(name)
, m_quietly(quietly)
{
}

uint32_t TestHarness::Run()
{
	assert(m_cases.size() == m_caseNames.size());

	uint32_t errors = 0;
	TestCase testCase;
	if(!m_quietly)
	{
		cout
			<< "Test: " << m_name << endl;
	}

	for(size_t i=0;i<m_cases.size(); ++i)
	{
		const TestFunction_t& test = m_cases[i];
		const string& testName = m_caseNames[i];

		if(!m_quietly)
		{
			cout << "    " << testName;
		}

		try
		{
			test(testCase);
			if(!m_quietly)
			{
				cout << " [PASSED]" << endl;
			}
		}
		catch(const TestCase::AssertionException& ex)
		{
			if(!m_quietly)
			{
				cout
					<< " [FAILED]" << endl
					<< "        Error: " << ex.GetMessage() << endl;
			}
			++errors;
		}
	}
	return errors;
}

void TestHarness::It(const string& caseName, TestFunction_t testFunction)
{
	for(const auto& cn : m_caseNames)
	{
		assert(cn != caseName);
	}
	m_cases.push_back(testFunction);
	m_caseNames.push_back(caseName);
}

CLOSE_NAMESPACE(Elf);