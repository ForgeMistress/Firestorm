///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestCase.h
//
//  Abstract non-instantiable base class single test case.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
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
		AssertionException(const string& message)
		: m_message(message) {}

		const string& GetMessage() const { return m_message; }

	private:
		string m_message;
	};

	void Assert(bool assertion, const string& message);

	void AssertIsTrue(bool assertion, const string& message);

	void AssertIsFalse(bool assertion, const string& message);

	template<class T>
	void AssertIsNull(T ptr, const string& message);

	template<class T>
	void AssertNotNull(T ptr, const string& message);


	size_t GetFailureCount() const;
	const vector<string>& GetFailures() const { return m_failures; }

protected:
	TestCase(const string& name, class TestHarness* harness)
		: _name(name)
		, _harness(harness) {}
	~TestCase() {}

	friend class TestHarness;

private:
	class TestHarness* _harness;
	vector<string> m_failures;
	string _name;
	bool _hasFailed{ false };
};

template<class T>
void TestCase::AssertNotNull(T ptr, const string& message)
{
	if(ptr == nullptr)
	{
		//throw AssertionException(message);
		m_failures.push_back(message);
	}
}

template<class T>
void TestCase::AssertIsNull(T ptr, const string& message)
{
	if(ptr != nullptr)
	{
		//throw AssertionException(message);
		m_failures.push_back(message);
	}
}

CLOSE_NAMESPACE(Firestorm);
#endif