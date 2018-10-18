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

OPEN_NAMESPACE(Firestorm);

/**
	\class GoWidePool

	A ThreadPool that is used to go wide and distribute work amongst multiple threads.
	The threads share work via work stealing.
 **/
class GoWidePool
{

};

class Node
{
	template<class ThreadPool> friend class TaskGraph;
public:
	template<class Function>
	Node(class TaskGraph& graph, size_t index, Function&& func)
	: _graph(graph)
	, _index(index)
	, _operation(forward<Function>(func))
	{
	}

	/**
		Invoke the held operation.
	 **/
	inline void Invoke() const
	{
		_operation();
	}

private:
	class TaskGraph& _graph;
	size_t _index;
	function<void()> _operation;
	vector<size_t> _children;
};

template<class ThreadPool>
class TaskGraph final
{
public:
	TaskGraph();

	/**
		Push a new node into the queue.
	 **/
	template<class Function>
	Node& Push(Function&& function)
	{
		size_t index = _nodes.size();
		_nodes.push_back(Node<Function>(forward<Function>(function)));
		return _nodes.back();
	}

	/**
		Check if two nodes are linked.
	 **/
	bool AreLinked(const Node& from, const Node& to);

	/**
		Clear the work queue and end the task tree. Calls ThreadPool::Join.
	 **/
	void Shutdown();

private:
	friend class Node;

	// connects from->to in the graph.
	void Link(const Node& from, const Node& to);
	Node& GetNode(size_t index);

	ThreadPool _threadPool;

	string _name;
	mutex _queueLock;

	// vector<std::thread> _threads;
	// queue<function<void(void)>> _queue;
	// std::condition_variable _cv;
	// bool _quit{ false };

	vector<Node>                 _nodes;
	vector<pair<size_t, size_t>> _edges;
};

template<class ThreadPool>
TaskGraph<ThreadPool>::TaskGraph()
{
}

template<class ThreadPool>
bool TaskGraph<ThreadPool>::AreLinked(const Node& from, const Node& to)
{
	for(const auto& edge : _edges)
	{
		// check if they're already linked.
		if(edge.first == from._index)
			if(edge.second == to._index)
				return true;
	}
	return false;
}

template<class ThreadPool>
void TaskGraph<ThreadPool>::Shutdown()
{
	_threadPool.Join();
}

template<class ThreadPool>
void TaskGraph<ThreadPool>::Link(const Node& from, const Node& to)
{
	if(AreLinked(from, to))
		return; // no-op if the nodes are already linked.
	_edges.push_back(from._index, to._index);
}

template<class ThreadPool>
Node& TaskGraph<ThreadPool>::GetNode(size_t index)
{
	FIRE_ASSERT(index < _nodes.size());
	return _nodes[index];
}

CLOSE_NAMESPACE(Firestorm);

#endif