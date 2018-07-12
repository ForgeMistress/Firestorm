///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///  \file ObjectPool.h
///
///  Contains \ref ObjectPool.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_OBJECTPOOL_H_
#define LIBEXISTENCE_OBJECTPOOL_H_
#pragma once

#include <libCore/Result.h>

OPEN_NAMESPACE(Elf);

struct Pool
{
	enum ErrorCodes : int
	{
		OBJECT_NOT_FOUND,
		POOL_FULL,
		POOL_EMPTY
	};
};

/**
	\class ObjectPool

	Provides access to a reusable pile of objects that remain in a contiguous area of memory.
	This class is cobbled together from concepts outlined in the following links:
	https://stackoverflow.com/questions/27827923/c-object-pool-that-provides-items-as-smart-pointers-that-are-returned-to-pool
	http://gameprogrammingpatterns.com/object-pool.html
 **/
template <class Object_t>
class ObjectPool
{
	typedef WeakPtr<ObjectPool<Object_t>* > PoolWeakPtr_t;
	typedef SharedPtr<ObjectPool<Object_t>* > PoolSharedPtr_t;

	// deleter for a shared ptr to an object.
	struct SharedPtrDeleter
	{
		explicit SharedPtrDeleter(PoolWeakPtr_t pool)
		: m_pool(pool)
		{
		}
		void operator()(Object_t* ptr)
		{
			if(auto poolPtr = m_pool.lock())
			{
				try {
					(*poolPtr.get())->Return(ptr);
				}
				catch (...) {}
			}
		}
		PoolWeakPtr_t m_pool;
	};

public:
	ObjectPool(uint32_t poolSize);
	virtual ~ObjectPool();
	
	template<class... Args_t>
	Result<SharedPtr<Object_t>, Error> Acquire(Args_t... args);

	void Return(const SharedPtr<Object_t>& obj)
	{
		Return(obj.get());
	}

	/**
		\function Return

	 **/
	void Return(Object_t* obj);

	/**
		\function IsInUse

		Check whether or not the object passed in is in use.

		\arg \c Object a raw pointer to the object we're checking on.
		\return \c Result return a result with the value being a bool.
	**/
	Result<bool, Error> IsInUse(const SharedPtr<Object_t>& obj)
	{
		return IsInUse(obj.get());
	}

	/**
		\function IsInUse

		Check whether or not the object passed in is in use.

		\arg \c Object a raw pointer to the object we're checking on.
		\return \c Result return a result with the value being a bool.
	 **/
	Result<bool, Error> IsInUse(Object_t* obj);

	/**
		\function GetObjectBlockSize

		Retrieve the amount of memory used by the block that holds the objects.
	 **/
	uint32_t GetObjectBlockSize();

	/**
		\function GetFlagBlockSize

		Retrieve the size of the flag block.
	 **/
	uint32_t GetFlagBlockSize();

private:
	Result<uint16_t, Error> FindUnused();
	Result<uint16_t, Error> Find(Object_t* obj);

	Object_t* Get(uint16_t index);
	void SetInUse(Object_t* obj);
	void SetAvailable(Object_t* obj);

	PoolSharedPtr_t  m_thisPtr;
	uint32_t         m_poolSize;
	Vector<Object_t> m_pool;
	Vector<Object_t> m_inUse;
};

template <class Object_t>
template <class... Args_t>
Result<SharedPtr<Object_t>, Error> ObjectPool<Object_t>::Acquire(Args_t... args)
{
	Result<uint16_t, Error> unused = FindUnused();
	if (unused.has_value())
	{
		Object_t* obj = Get(unused.value());
		SetInUse(obj);
		assert(obj);
		try {
			obj->Recycle(args...);
		}
		catch (...) {}
		return SharedPtr<Object_t>(obj, SharedPtrDeleter(m_thisPtr));
	}
	return tl::make_unexpected(unused.error());
}

template <class Object_t>
ObjectPool<Object_t>::ObjectPool(uint32_t poolSize)
: m_thisPtr(new ObjectPool<Object_t>*(this))
, m_poolSize(poolSize)
{
	m_pool.reserve(poolSize);
	m_inUse.reserve(poolSize);
	for(uint16_t i = 0; i < poolSize; ++i)
	{
		m_pool[i].Init();
		m_inUse[i] = false;
	}
}

template <class Object_t> 
ObjectPool<Object_t>::~ObjectPool()
{
	for(uint32_t i = 0; i < m_poolSize; ++i)
	{
		// tell the pooled objects that they're getting deleted.
		try {
			m_pool[i].Delete();
		}
		catch (...) {}
	}
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> void ObjectPool<Object_t, PoolSize_v>::Return(Object_t* obj)
{
	SetAvailable(obj);
}

template <class Object_t>
Result<bool, Error> ObjectPool<Object_t>::IsInUse(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		return m_inUse[f.value()];
	}
	return tl::make_unexpected(f.error());
}

template <class Object_t>
uint32_t ObjectPool<Object_t>::GetObjectBlockSize()
{
	return sizeof(m_pool);
}

template <class Object_t>
uint32_t ObjectPool<Object_t>::GetFlagBlockSize()
{
	return sizeof(m_inUse);
}

template <class Object_t>
Result<uint16_t, Error> ObjectPool<Object_t>::FindUnused()
{
	for(uint32_t i = 0; i < m_poolSize; ++i)
	{
		if(m_inUse[i] == false)
		{
			return i;
		}
	}
	return tl::make_unexpected(Error(Pool::POOL_EMPTY, "the pool does not contain any more unused objects"));
}

template <class Object_t>
Object_t* ObjectPool<Object_t>::Get(uint16_t index)
{
	assert(index < PoolSize_v);
	return &m_pool[index];
}

template <class Object_t>
void ObjectPool<Object_t>::SetInUse(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		m_inUse[f.value()] = true;
	}
}

template <class Object_t>
void ObjectPool<Object_t>::SetAvailable(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		m_inUse[f.value()] = false;
	}
}

template<class Object_t>
Result<uint16_t, Error> ObjectPool<Object_t>::Find(Object_t* obj)
{
	for(uint32_t i = 0; i < m_poolSize; ++i)
	{
		if(Get(i) == obj)
		{
			return i;
		}
	}
	return tl::make_unexpected(Error(Pool::OBJECT_NOT_FOUND, "object could not be found"));
}

CLOSE_NAMESPACE(Elf);
#endif