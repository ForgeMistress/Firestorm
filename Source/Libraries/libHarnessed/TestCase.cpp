#include "stdafx.h"
#include "TestCase.h"

OPEN_NAMESPACE(Elf);

void TestCase::Assert(bool assertion, const String& message)
{
	if(!assertion)
	{
		throw AssertionException(message);
	}
}

size_t TestCase::GetFailureCount() const
{
	return m_failures.size();
}

CLOSE_NAMESPACE(Elf);