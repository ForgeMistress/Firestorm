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
#include "Object.h"
#include "EventDispatcher.h"
#include <taskflow/taskflow.hpp>

OPEN_NAMESPACE(Firestorm);

class TaskGraph
{
public:
	TaskGraph(class Application& app);

	void Shutdown();

	tf::Task& operator[](const char* name);

	void wait_for_all();

	std::shared_future<void> dispatch();

	bool has(const char* name) const;

	template<class C>
	auto emplace(C&& c, const char* name = nullptr)
	{
		auto [task, fut] = _tf.emplace(eastl::forward<C>(c));
		if(name)
		{
			task.name(name);
			_graphs[std::string(name)] = task;
		}
		return eastl::make_pair(task, eastl::move(fut));
	}

	template<class C>
	auto silent_emplace(C&& c, const char* name = nullptr)
	{
		auto task = _tf.silent_emplace(std::forward<C>(c));
		if(name)
		{
			task.name(name);
			_graphs[std::string(name)] = task;
		}
		return task;
	}

	template<class C>
	auto silent_emplace(C&& c)
	{
		return _tf.silent_emplace(std::forward<C>(c));
	}

	template<class Arg, class F>
	EventDispatcher::Receipt Register(F&& func)
	{
		return _dispatcher.Register<Arg>(eastl::forward<F>(f));
	}

	void clear()
	{
		_graphs.clear();
	}

private:
	// we have to use std:: stuff for now because of the tf reliance on it.
	class Application&                        _app;
	tf::Taskflow                              _tf;
	std::unordered_map<std::string, tf::Task> _graphs;
	EventDispatcher _dispatcher;
};

struct GraphDispatchedMessage
{
	FIRE_MIRROR_DECLARE(GraphDispatchedMessage);
public:
	GraphDispatchedMessage(TaskGraph& graph)
		: Graph(graph){}
	// convenience reference.
	TaskGraph& Graph;
};

CLOSE_NAMESPACE(Firestorm);

#endif