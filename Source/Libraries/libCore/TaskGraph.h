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

#include "libCore.h"
#include <EASTL/bonus/tuple_vector.h>

OPEN_NAMESPACE(Firestorm);

/**
	\class GoWidePool

	A ThreadPool that is used to go wide and distribute work amongst multiple threads.
	The threads share work via work stealing.
 **/
class GoWidePool
{

};

using Node = size_t;

class TaskGraph final
{
public:
	TaskGraph();

	/**
		Push a new node into the queue.
	 **/
	template<class Function>
	Node Push(Function&& function)
	{
		size_t index = _nodes.size();
		_nodes.push_back(forward<Function>(function), vector<Node>());
		return index;
	}

	/**
		Check if two nodes are linked.
	 **/
	bool AreLinked(Node from, Node to);

	/**
		Clear the work queue and end the task tree. Calls ThreadPool::Join.
	 **/
	void Shutdown();

private:
	// connects from->to in the graph.
	void Link(Node from, Node to);

	string _name;
	mutex _queueLock;

	// vector<std::thread> _threads;
	// queue<function<void(void)>> _queue;
	// std::condition_variable _cv;
	// bool _quit{ false };

	using NodeEdge = pair<Node, Node>;
	using NodeChildList = pair<Node, vector<Node>>;

	tuple_vector<function<void()>, NodeChildList> _nodes;
	vector<NodeEdge>                              _edges;        // edges between nodes.
};

TaskGraph::TaskGraph()
{
}

bool TaskGraph::AreLinked(Node from, Node to)
{
	for(const auto& edge : _edges)
	{
		// check if they're already linked.
		if(edge.first == from && edge.second == to)
			return true;
	}
	return false;
}

void TaskGraph::Shutdown()
{
}

void TaskGraph::Link(Node from, Node to)
{
	if(AreLinked(from, to))
		return; // no-op if the nodes are already linked.
	_edges.push_back(pair(from, to));
}

CLOSE_NAMESPACE(Firestorm);

#endif