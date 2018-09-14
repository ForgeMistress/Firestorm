///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SynchronizedQueue
//
//  An implementation of a thread safe queue structure.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_SYNCHRONIZEDQUEUE_H_
#define LIBCORE_SYNCHRONIZEDQUEUE_H_
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

OPEN_NAMESPACE(Firestorm);

template <class T>
class SynchronizedQueue
{
public:
	T pop()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		while(_queue.empty())
		{
			_cond.wait(lock);
		}
		auto item = _queue.front();
		_queue.pop();
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		while (_queue.empty())
		{
			_cond.wait(lock);
		}
		item = _queue.front();
		_queue.pop();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_queue.push(item);
		lock.unlock();
		_cond.notify_one();
	}

	void push(T&& item)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_queue.push(std::move(item));
		lock.unlock();
		_cond.notify_one();
	}
private:
	std::queue<T> _queue;
	std::mutex _mutex;
	std::condition_variable _cond;
};

CLOSE_NAMESPACE(Firestorm);

#endif