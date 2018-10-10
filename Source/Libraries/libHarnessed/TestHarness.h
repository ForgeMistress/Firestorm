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
	typedef Function<void(TestCase&)> TestFunction_t;
	typedef Function<void(Benchmark&)> BenchmarkFunction_t;

	TestHarness(const String& name, bool quietly = false);
	virtual ~TestHarness();

	// Run before every test case.
	virtual void Before() {}

	// Runs all of the test cases. Reports back the number of errors.
	uint32_t Run();

	// Inserts a test case. The name should be something descriptive and should be a
	// sentence that states what it should be doing.
	//
	// Example: It("should do this thing properly without errors", ...);
	void It(const String& name, TestFunction_t testFunction);

	void Profile(const String& name, size_t numberOfRuns, BenchmarkFunction_t benchmarkFunction);

    const String& GetName() const { return _name; }

	template <class... Args_t>
	void Print(const char* fmt, Args_t... args)
	{
		if(!_quietly)
			FIRE_LOG_ERROR(fmt, args...);
	}

	void ReportError(TestCase* tc, const String& message);

private:
	size_t RunTests();
	void RunBenchmarks();


	String _name;
	bool _quietly;
	Vector<String>         _caseNames;
	Vector<TestFunction_t> _cases;

	struct BenchmarkInfo
	{
		size_t NumRuns;
		String Name;
		BenchmarkFunction_t Op;
	};
	Vector<BenchmarkInfo> _benchmarks;
};

CLOSE_NAMESPACE(Firestorm);

#endif
