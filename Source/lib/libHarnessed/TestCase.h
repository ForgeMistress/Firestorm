///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestCase.h
//
//  Abstract non-instantiable base class single test case.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef HARNESSED_TESTCASE_H_
#define HARNESSED_TESTCASE_H_
#pragma once

OPEN_NAMESPACE(Elf);

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
	size_t GetFailureCount() const;

protected:
	TestCase() {}
	~TestCase() {}

	friend class TestHarness;

private:
	vector<string> m_failures;
};

CLOSE_NAMESPACE(Elf);
#endif