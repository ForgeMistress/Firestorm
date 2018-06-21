///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestCase.h
//
//  Abstract non-instantiable base class single test case.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBHARNESSED_TESTCASE_H_
#define LIBHARNESSED_TESTCASE_H_
#pragma once

OPEN_NAMESPACE(Elf);

class TestCase
{
public:
	struct AssertionException
	{
		AssertionException(const String& message)
		: m_message(message) {}

		const String& GetMessage() const { return m_message; }

	private:
		String m_message;
	};

	void Assert(bool assertion, const String& message);

	template<class T>
	void AssertNotNull(T ptr, const String& message);


	size_t GetFailureCount() const;

protected:
	TestCase() {}
	~TestCase() {}

	friend class TestHarness;

private:
	Vector<String> m_failures;
};

CLOSE_NAMESPACE(Elf);
#endif