#include "stdafx.h"
#include "TestCase.h"
#include "TestHarness.h"

OPEN_NAMESPACE(Firestorm);

void TestCase::Assert(bool assertion, const string& message)
{
	if(!assertion)
	{
		m_failures.push_back(message);
		_hasFailed = true;
		_harness->ReportError(this, message);
	}
}

void TestCase::AssertIsTrue(bool assertion, const string& message)
{
	if(!assertion)
	{
		m_failures.push_back(message);
		_hasFailed = true;
		_harness->ReportError(this, message);
	}
}

void TestCase::AssertIsFalse(bool assertion, const string& message)
{
	if(assertion)
	{
		m_failures.push_back(message);
		_hasFailed = true;
		_harness->ReportError(this, message);
	}
}

size_t TestCase::GetFailureCount() const
{
	return m_failures.size();
}

CLOSE_NAMESPACE(Firestorm);