///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestHarness.h
//
//  Abstract base class that holds a collection of TestCase instances.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef HARNESSED_TESTHARNESS_H_
#define HARNESSED_TESTHARNESS_H_
#pragma once

OPEN_NAMESPACE(Elf);

class TestCase;

class TestHarness
{
public:
	typedef std::function<void (TestCase&)> TestFunction_t;

	TestHarness(const string& name, bool quietly = false);
	virtual ~TestHarness() {}

	// Run before every test case.
	virtual void Before() {}

	// Runs all of the test cases. Reports back the number of errors.
	uint32_t Run();

	// Inserts a test case. The name should be something descriptive and should be a
	// sentence that states what it should be doing.
	//
	// Example: It("should do this thing properly without errors", ...);
	void It(const string& name, TestFunction_t testFunction);

private:
	string m_name;
	bool m_quietly;
	vector<string>         m_caseNames;
	vector<TestFunction_t> m_cases;
};

CLOSE_NAMESPACE(Elf);

#endif