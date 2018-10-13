///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Benchmark
//
//  An object that stores snapshots of a benchmark test.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Benchmark.h"
#include <libCore/Logger.h>
#include <iomanip>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Benchmark::Benchmark(const string& name, size_t numRuns, size_t index)
: _name(name)
, _numRuns(numRuns)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Benchmark::~Benchmark()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Benchmark::Run(function<void(Benchmark&)> op)
{
	_overallStart = clock_type::now();
	for(size_t i = 0; i < _numRuns; ++i)
	{
		op(*this);
	}
	_overallStop = clock_type::now();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Benchmark::Report()
{
	size_t widest = string("Overall Time").size();
	for(auto result : _snapshotResults)
	{
		if(result.Name.size() > widest)
			widest = result.Name.size();
	}

	FIRE_LOG_DEBUG("===== Time Averages Over %d Runs (In Seconds) =====", _numRuns);

	for(auto result : _snapshotResults)
	{
		size_t diff = widest - result.Name.size();
		string spacePadding;
		for(size_t i = 0; i < diff; ++i)
		{
			spacePadding.push_back(' ');
		}

		double total = 0;
		for(size_t i = 0; i < result.Results.size(); ++i)
		{
			total += result.Results[i];
		}
		FIRE_LOG_DEBUG("%s%s = %.10f", result.Name, spacePadding, total / _numRuns);
	}
	string name("Overall Time");
	size_t diff = widest - name.size();
	string spacePadding;
	for(size_t i = 0; i < diff; ++i)
	{
		spacePadding.push_back(' ');
	}
	auto cnt = eastl::chrono::duration_cast<eastl::chrono::nanoseconds>(
		eastl::chrono::duration<double, eastl::nano>(_overallStop - _overallStart)
		).count();
	double overallTime = ((double)cnt / 1000000000.0);
	FIRE_LOG_DEBUG("%s%s = %.10f", name, spacePadding, overallTime);
	FIRE_LOG_DEBUG("");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);