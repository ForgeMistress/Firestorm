///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ObjectPool
//
//  Implements a pool of objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_OBJECTPOOL_H_
#define LIBCORE_OBJECTPOOL_H_
#pragma once

#include <forward_list>

OPEN_NAMESPACE(Firestorm);

template <class T>
class ObjectPool final
{
	struct Deleter
	{
		Deleter(ObjectPool<T>& recycle);
		void operator()(T* ptr);
		ObjectPool<T>& _recycle;
	};
public:
	using Handle = std::unique_ptr<T, Deleter>;

	template<class... Args_t>
	T* Get(Args_t&&... args)
	{
		if(_recycle.empty())
		{
			T& item = _pool.emplace_front(std::forward<Args_t>(args)...);
			return &item;
		}
		auto item = _recycle.back();
		_recycle.pop_back();
		new (item) T(std::forward<Args_t>(args)...);
		return item;
	}

	template<class... Args_t>
	Handle GetManaged(Args_t&&... args)
	{
		return Handle(Get(args), Deleter(_recycle));
	}

	void Return(T* ptr)
	{
		if(ptr != nullptr)
		{
			ptr->~T();
			_recycle.push_back(ptr);
		}
	}

private:
	std::forward_list<T> _pool;
	Vector<T*> _recycle;
};

template<class T>
ObjectPool<T>::Deleter::Deleter(ObjectPool<T>& recycle)
: _recycle(recycle)
{
}

template<class T>
void ObjectPool<T>::Deleter::operator()(T* ptr)
{
	_recycle.Return(ptr);
}

CLOSE_NAMESPACE(Firestorm);

#endif