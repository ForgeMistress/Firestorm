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

OPEN_NAMESPACE(Firestorm);

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

	void AssertIsTrue(bool assertion, const String& message);

	void AssertIsFalse(bool assertion, const String& message);

	template<class T>
	void AssertIsNull(T ptr, const String& message);

	template<class T>
	void AssertNotNull(T ptr, const String& message);


	size_t GetFailureCount() const;
	const Vector<String>& GetFailures() const { return m_failures; }

protected:
	TestCase(const String& name, class TestHarness* harness)
		: _name(name)
		, _harness(harness) {}
	~TestCase() {}

	friend class TestHarness;

private:
	class TestHarness* _harness;
	Vector<String> m_failures;
	String _name;
	bool _hasFailed{ false };
};

template<class T>
void TestCase::AssertNotNull(T ptr, const String& message)
{
	if(ptr == nullptr)
	{
		//throw AssertionException(message);
		m_failures.push_back(message);
	}
}

template<class T>
void TestCase::AssertIsNull(T ptr, const String& message)
{
	if(ptr != nullptr)
	{
		//throw AssertionException(message);
		m_failures.push_back(message);
	}
}

CLOSE_NAMESPACE(Firestorm);
#endif