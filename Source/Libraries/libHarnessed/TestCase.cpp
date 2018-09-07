#include "stdafx.h"
#include "TestCase.h"

OPEN_NAMESPACE(Firestorm);

void TestCase::Assert(bool assertion, const String& message)
{
	if(!assertion)
	{
		throw AssertionException(message);
	}
}

template<class T>
void TestCase::AssertNotNull(T ptr, const String& message)
{
	if(!ptr)
	{
		throw AssertionException(message);
	}
}

size_t TestCase::GetFailureCount() const
{
	return m_failures.size();
}

CLOSE_NAMESPACE(Firestorm);