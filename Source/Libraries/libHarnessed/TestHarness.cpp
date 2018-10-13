///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TestHarness.cpp
//
//  Abstract base class that holds a collection of TestCase instances.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TestHarness.h"
#include "TestCase.h"
#include "Benchmark.h"

OPEN_NAMESPACE(Firestorm);

TestHarness::TestHarness(const string& name, bool quietly)
: _name(name)
, _quietly(quietly)
{
}

TestHarness::~TestHarness()
{
}

size_t TestHarness::Run()
{
	size_t numFailures = RunTests();
	RunBenchmarks();
	return numFailures;
}

void TestHarness::It(const string& caseName, TestFunction_t testFunction)
{
	for(const auto& cn : _caseNames)
	{
		FIRE_ASSERT(cn != caseName);
	}
	_cases.push_back(testFunction);
	_caseNames.push_back(caseName);
}

void TestHarness::Profile(const string& name, size_t numberOfRuns, BenchmarkFunction_t benchmarkFunction)
{
	for(const auto& bm : _benchmarks)
	{
		if(bm.Name == name)
			throw AssertionException("benchmark with the same name already exists");
	}
	_benchmarks.push_back(BenchmarkInfo{
		numberOfRuns,
		name,
		benchmarkFunction
	});
}

void TestHarness::ReportError(TestCase* tc, const string& message)
{
	Print("    Error: %s", message);
}


size_t TestHarness::RunTests()
{
	FIRE_ASSERT(_cases.size() == _caseNames.size());

	Print("Test: %s", _name.c_str());
	Print("");

	size_t finalErrorCount = 0;
	bool hasFailed = false;
	for(size_t i=0;i<_cases.size(); ++i)
	{
		auto test = _cases[i];
		const string& testName = _caseNames[i];

		Print("Case[%d] %s", i, testName.c_str());
		TestCase testCase(testName, this);

		try
		{
			test(testCase);
		}
		catch(AssertionException& assertion)
		{
			Print("!! Case [%d] failed due to an internal assertion and kicked out early...", i);
			Print("!! Case [%d] may have additional issues that were not detected due to the kickout. ", i);
			Print("!! Case [%d] fix the assert and run the tests again.", i);
			Print("!!    %s", assertion.Report());
			finalErrorCount += 1;
		}
		auto failures = testCase.m_failures;
		if(!failures.empty())
		{
			finalErrorCount += failures.size();
			Print("!! Case [%d] failed with %d errors...", i, failures.size());
		}
	}
	return finalErrorCount;
}

void TestHarness::RunBenchmarks()
{
	for(size_t i = 0; i < _benchmarks.size(); ++i)
	{
		auto benchmark = _benchmarks[i];
		const string& benchmarkName = benchmark.Name;

		Print("Benchmark[%d] %s", i, benchmarkName.c_str());
		Benchmark b(benchmarkName, benchmark.NumRuns, i);

		b.Run(benchmark.Op);

		b.Report();
	}
}

CLOSE_NAMESPACE(Firestorm);