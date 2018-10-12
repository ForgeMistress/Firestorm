///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestHarness.h
//
//  Abstract base class that holds a collection of TestCase instances.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBHARNESSED_TESTHARNESS_H_
#define LIBHARNESSED_TESTHARNESS_H_
#pragma once

#include <libCore/Logger.h>

OPEN_NAMESPACE(Firestorm);

class TestCase;
class Benchmark;

class TestHarness final
{
public:
	typedef function<void(TestCase&)> TestFunction_t;
	typedef function<void(Benchmark&)> BenchmarkFunction_t;

	TestHarness(const string& name, bool quietly = false);
	virtual ~TestHarness();

	// Run before every test case.
	virtual void Before() {}

	// Runs all of the test cases. Reports back the number of errors.
	uint32_t Run();

	// Inserts a test case. The name should be something descriptive and should be a
	// sentence that states what it should be doing.
	//
	// Example: It("should do this thing properly without errors", ...);
	void It(const string& name, TestFunction_t testFunction);

	void Profile(const string& name, size_t numberOfRuns, BenchmarkFunction_t benchmarkFunction);

    const string& GetName() const { return _name; }

	template <class... Args_t>
	void Print(const char* fmt, Args_t... args)
	{
		if(!_quietly)
			FIRE_LOG_ERROR(fmt, args...);
	}

	void ReportError(TestCase* tc, const string& message);

private:
	size_t RunTests();
	void RunBenchmarks();


	string _name;
	bool _quietly;
	vector<string>         _caseNames;
	vector<TestFunction_t> _cases;

	struct BenchmarkInfo
	{
		size_t NumRuns;
		string Name;
		BenchmarkFunction_t Op;
	};
	vector<BenchmarkInfo> _benchmarks;
};

CLOSE_NAMESPACE(Firestorm);

#endif
