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
#include "SOA.h"

OPEN_NAMESPACE(Firestorm);

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
		_nodes.push_back(index, forward<Function>(function), index);
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

	SOA<Node, function<void()>, Node> _nodes; // ID, operation, parent.
	vector<NodeEdge>                  _edges; // edges between nodes.
};

CLOSE_NAMESPACE(Firestorm);

#endif