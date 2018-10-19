///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TaskGraph
//
//  Implements an asynchronous task graph.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TaskGraph.h"
#include "Assert.h"
#include "transwarp.h"

#include <libApp/Application.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

TaskGraph::TaskGraph(Application& app)
: _app(app)
, _executor(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskGraph::Shutdown()
{
	delete _executor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Task& TaskGraph::get(const char* name) const
{
	auto found = _graphs.find(name);
	if(found != _graphs.end())
	{
		return (*found).second;
	}
	static Task _s_task;
	return _s_task;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskGraph::operate(const char* name, bool resetAll)
{
	auto found = _graphs.find(name);
	if(found != _graphs.end())
	{
		(*found).second->schedule_all(*_executor, resetAll);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TaskGraph::erase(const char* name)
{
	auto found = _graphs.find(name);
	if(found != _graphs.end())
	{
		_graphs.erase(name);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);