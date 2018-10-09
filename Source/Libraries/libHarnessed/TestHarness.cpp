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

TestHarness::TestHarness(const String& name, bool quietly)
: m_name(name)
, m_quietly(quietly)
{
}

TestHarness::~TestHarness()
{
}

uint32_t TestHarness::Run()
{
	size_t numFailures = RunTests();
	RunBenchmarks();
	return numFailures;
}

void TestHarness::It(const String& caseName, TestFunction_t testFunction)
{
	for(const auto& cn : m_caseNames)
	{
		FIRE_ASSERT(cn != caseName);
	}
	_cases.push_back(testFunction);
	m_caseNames.push_back(caseName);
}

void TestHarness::Profile(const String& name, size_t numberOfRuns, BenchmarkFunction_t benchmarkFunction)
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

void TestHarness::ReportError(TestCase* tc, const String& message)
{
	Print("    Error: %s", message);
}


size_t TestHarness::RunTests()
{
	FIRE_ASSERT(_cases.size() == m_caseNames.size());

	Print("Test: %s", m_name);
	Print("");

	uint32_t finalErrorCount = 0;
	bool hasFailed = false;
	for(size_t i=0;i<_cases.size(); ++i)
	{
		auto test = _cases[i];
		const String& testName = m_caseNames[i];

		Print("Case[%d] %s",i, testName);
		TestCase testCase(testName, this);

		test(testCase);
		auto failures = testCase.m_failures;
		if(!failures.empty())
		{
			finalErrorCount += failures.size();
			Print("!!  Case [%d] failed with %d errors...", i, failures.size());
		}
	}
	return finalErrorCount;
}

void TestHarness::RunBenchmarks()
{
	for(size_t i = 0; i < _benchmarks.size(); ++i)
	{
		auto benchmark = _benchmarks[i];
		const String& benchmarkName = benchmark.Name;

		Print("Benchmark[%d] %s", i, benchmarkName);
		Benchmark b(benchmarkName, benchmark.NumRuns, i);

		b.Run(benchmark.Op);

		b.Report();
	}
}

CLOSE_NAMESPACE(Firestorm);