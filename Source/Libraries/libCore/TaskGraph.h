///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TaskGraph
//
//  Implements an asynchronous task graph.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_TASKGRAPH_H_
#define LIBCORE_TASKGRAPH_H_
#pragma once

#include "Common.h"
#include <taskflow/taskflow.hpp>

OPEN_NAMESPACE(Firestorm);

class TaskGraph
{
public:
	TaskGraph(class Application& app);

	void Shutdown();

	tf::Task operator[](const char* name);

	void wait_for_all();

	std::shared_future<void> dispatch();

	bool has(const char* name) const;

	template<class C>
	auto emplace(const char* name, C&& c) const
	{
		auto [task, fut] = _tf.emplace(std::forward<C>(c));
		task.name(name);
		_graphs[string(name)] = task;
		return std::make_tuple(task, fut);
	}

	template<class C>
	auto emplace(C&& c)
	{
		return _tf.emplace(std::forward<C>(c));
	}

	template<class C>
	auto silent_emplace(const char* name, C&& c) const
	{
		auto task = _tf.silent_emplace(std::forward<C>(c));
		task.name(name);
		_graphs[string(name)] = task;
		return task;
	}

	template<class C>
	auto silent_emplace(C&& c)
	{
		return _tf.silent_emplace(std::forward<C>(c));
	}

private:
	class Application&              _app;
	tf::Taskflow                    _tf;
	eastl::unordered_map<eastl::string, tf::Task> _graphs;
};

CLOSE_NAMESPACE(Firestorm);

#endif