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

#include "Assert.h"
#include <forward_list>

OPEN_NAMESPACE(Firestorm);

template<class T> class ObjectPool;

template<class T>
class PoolPtr final
{
public:
	PoolPtr(T* ptr, ObjectPool<T>& pool);
	PoolPtr(PoolPtr&& other);

	~PoolPtr();

	/**
		Release the held pointer without recycling it. DANGEROUS! DO NOT DO THIS IF YOU DON'T KNOW
		WHAT THE IMPLICATIONS ARE. YOU ARE RESPONSIBLE FOR RETURNING THE POINTER TO THE POOL
		WHEN YOU ARE DONE WITH IT!
	 **/
	T* Release();

	/**
		Return the held pointer to the pool.
	 **/
	void Recycle();

	/**
		Return whether or not this PoolPtr holds an actual honest to the gods live pointer.
	 **/
	bool IsValid() const;

	T* operator->()
	{
		return _ptr;
	}

	const T* operator->() const
	{
		return _ptr;
	}

private:
	// move only because reasons.
	// oh, and no move assignment either you plebian.
	PoolPtr(const PoolPtr&) = delete;
	PoolPtr& operator=(const PoolPtr&) = delete;
	PoolPtr& operator=(PoolPtr&& other) = delete;

	T* _ptr{ nullptr };
	ObjectPool<T>& _pool;
};

template <class T>
class ObjectPool final
{
public:
	template<class... Args_t>
	T* Get(Args_t&&... args);

	template<class... Args_t>
	PoolPtr<T> GetManaged(Args_t&&... args);

	template<class U>
	void Return(U* ptr);

private:
	std::forward_list<T> _pool;
	Vector<T*> _recycle;
};

template<class T>
PoolPtr<T>::PoolPtr(T* ptr, ObjectPool<T>& pool)
: _ptr(ptr)
, _pool(pool)
{
}

template<class T>
PoolPtr<T>::~PoolPtr()
{
	if (_ptr)
		_pool.Return(_ptr);
	_ptr = nullptr;
}

template<class T>
PoolPtr<T>::PoolPtr(PoolPtr&& other)
	: _ptr(other._ptr)
	, _pool(other._pool)
{
	other._ptr = nullptr;
}

template<class T>
T* PoolPtr<T>::Release()
{
	T* ptr = _ptr;
	_ptr = nullptr;
	return ptr;
}

template<class T>
void PoolPtr<T>::Recycle()
{
	if(_ptr)
	{
		_pool.Return(_ptr);
		_ptr = nullptr;
	}
}

template<class T>
bool PoolPtr<T>::IsValid() const
{
	return _ptr != nullptr;
}

template<class T>
template<class... Args_t>
T* ObjectPool<T>::Get(Args_t&&... args)
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

template<class T>
template<class... Args_t>
PoolPtr<T> ObjectPool<T>::GetManaged(Args_t&&... args)
{
	return PoolPtr<T>{ Get(args), *this };
}

template<class T>
template<class U>
void ObjectPool<T>::Return(U* ptr)
{
	static_assert(
		std::is_same<T, U>::value ||
		std::is_convertible<T, U>::value,
		"invalid type passed to ObjectPool::Return"
	);
	FIRE_ASSERT(ptr != nullptr);
	T* ptrT = static_cast<T*>(ptr);
	ptrT->~T();
	_recycle.push_back(ptrT);
}

CLOSE_NAMESPACE(Firestorm);

#endif