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

using Task = tf::Task;

class TaskGraph
{
public:
	TaskGraph(class Application& app);

	void Shutdown();

	const Task& get(const char* name) const;

	template<class Function, class... Parents>
	Task make_root(const char* name, Function&& func, const Parents&... parents)
	{
		if(_graphs.find(name) == _graphs.end())
		{
			//_graphs[name] = tw::make_task(tw::root, forward<Function>(func), forward<Parents>(parents)...);
			return _graphs[name];
		}
		return _graphs[name];
	}

	void operate(const char* name, bool resetAll = true);

	void erase(const char* name);
private:
	class Application&          _app;
	tf::Taskflow                _tf;
	unordered_map<string, Task> _graphs;
};

CLOSE_NAMESPACE(Firestorm);

#endif