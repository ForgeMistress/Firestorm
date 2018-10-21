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

OPEN_NAMESPACE(Firestorm);

TaskGraph::TaskGraph(class Application& app)
: _app(app)
, _tf(std::thread::hardware_concurrency())
{
}

tf::Task& TaskGraph::operator[](const char* name)
{
	return _graphs[std::string(name)];
}

void TaskGraph::Shutdown()
{
	_tf.wait_for_all();
	_graphs.clear();
}

void TaskGraph::wait_for_all()
{
	_tf.wait_for_all();
	_dispatcher.Dispatch<GraphDispatchedMessage>(GraphDispatchedMessage{ *this });
	_graphs.clear();
}

std::shared_future<void> TaskGraph::dispatch()
{
	_dispatcher.Dispatch<GraphDispatchedMessage>(GraphDispatchedMessage{ *this });
	return _tf.dispatch();
}

bool TaskGraph::has(const char* name) const
{
	return _graphs.find(std::string(name)) != _graphs.end();
}

CLOSE_NAMESPACE(Firestorm);