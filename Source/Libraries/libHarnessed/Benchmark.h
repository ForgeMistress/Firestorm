///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Benchmark
//
//  An object that stores snapshots of a benchmark test.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBHARNESSED_BENCHMARK_H_
#define LIBHARNESSED_BENCHMARK_H_
#pragma once

#include <libCore/ObjectPool.h>

OPEN_NAMESPACE(Firestorm);

class Benchmark final
{
	friend class TestHarness;
	using clock_type = eastl::chrono::high_resolution_clock;
	using time_point = eastl::chrono::time_point<clock_type>;
public:
	Benchmark(const string& name, size_t numRuns, size_t index);
	~Benchmark();

	void Run(function<void(Benchmark&)> op);

	void Report();

	struct SnapshotHandle
	{
		string name;
		time_point start;
		time_point stop;
	};
	inline SnapshotHandle* StartSegment(const string& name)
	{
		SnapshotHandle* handle = _handles.Get();
		handle->name = name;
		handle->start = clock_type::now();
		return handle;
	}

	inline void StopSegment(SnapshotHandle* ssh)
	{
		ssh->stop = clock_type::now();
		auto findFunc = [this, ssh](const Result& r){
			return r.Name == ssh->name;
		};
		auto found = std::find_if(_snapshotResults.begin(), _snapshotResults.end(),findFunc);
		if(found == _snapshotResults.end())
		{
			_snapshotResults.push_back(Result{
				ssh->name,
				vector<double>()
			});
		}
		found = std::find_if(_snapshotResults.begin(), _snapshotResults.end(), findFunc);
		FIRE_ASSERT(found != _snapshotResults.end());

		auto cnt = eastl::chrono::duration_cast<eastl::chrono::nanoseconds>(
			eastl::chrono::duration<double, std::nano>(ssh->stop - ssh->start)
		).count();
		(*found).Results.push_back((double)cnt / 1000000000.0);
		_handles.Return(ssh);
	}

private:
	ObjectPool<SnapshotHandle> _handles;
	size_t _numRuns;

	string _name;
	time_point _overallStart;
	time_point _overallStop;

	struct Result
	{
		string Name;
		vector<double> Results;
	};
	vector<Result> _snapshotResults;
};

CLOSE_NAMESPACE(Firestorm);

#endif