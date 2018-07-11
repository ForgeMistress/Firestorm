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
template <class Object_t, uint16_t PoolSize_v>
class ObjectPool
{
	typedef WeakPtr<ObjectPool<Object_t, PoolSize_v>* > PoolWeakPtr_t;
	typedef SharedPtr<ObjectPool<Object_t, PoolSize_v>* > PoolSharedPtr_t;

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
	ObjectPool();
	virtual ~ObjectPool();

	template<class... Args_t>
	Result<SharedPtr<Object_t>, Error> Acquire(Args_t... args)
	{
		Result<uint16_t, Error> unused = FindUnused();
		if(unused.has_value())
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

	void Return(Object_t* obj);

	Result<bool, Error> IsInUse(Object_t* obj);

	uint32_t GetObjectBlockSize();
	uint32_t GetFlagBlockSize();

private:
	Result<uint16_t, Error> FindUnused();
	Result<uint16_t, Error> Find(Object_t* obj);

	Object_t* Get(uint16_t index);
	void SetInUse(Object_t* obj);
	void SetAvailable(Object_t* obj);

	PoolSharedPtr_t m_thisPtr;
	Object_t        m_pool  [ PoolSize_v ];
	bool            m_inUse [ PoolSize_v ];
};

template <
	class    Object_t,
	uint16_t PoolSize_v
> ObjectPool<Object_t, PoolSize_v>::ObjectPool()
: m_thisPtr(new ObjectPool<Object_t, PoolSize_v>*(this))
{
	for(uint16_t i = 0; i < PoolSize_v; ++i)
	{
		m_pool[i].Init();
		m_inUse[i] = false;
	}
}

template <
	class    Object_t,
	uint16_t PoolSize_v
> ObjectPool<Object_t, PoolSize_v>::~ObjectPool()
{
	for (uint16_t i = 0; i < PoolSize_v; ++i)
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

template<
	class    Object_t,
	uint16_t PoolSize_v
> Result<bool, Error> ObjectPool<Object_t, PoolSize_v>::IsInUse(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		return m_inUse[f.value()];
	}
	return tl::make_unexpected(f.error());
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> uint32_t ObjectPool<Object_t, PoolSize_v>::GetObjectBlockSize()
{
	return sizeof(m_pool);
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> uint32_t ObjectPool<Object_t, PoolSize_v>::GetFlagBlockSize()
{
	return sizeof(m_inUse);
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> Result<uint16_t, Error> ObjectPool<Object_t, PoolSize_v>::FindUnused()
{
	for(uint16_t i = 0; i < PoolSize_v; ++i)
	{
		if(m_inUse[i] == false)
		{
			return i;
		}
	}
	return tl::make_unexpected(Error(Pool::POOL_EMPTY, "the pool does not contain any more unused objects"));
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> Object_t* ObjectPool<Object_t, PoolSize_v>::Get(uint16_t index)
{
	assert(index < PoolSize_v);
	return &m_pool[index];
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> void ObjectPool<Object_t, PoolSize_v>::SetInUse(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		m_inUse[f.value()] = true;
	}
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> void ObjectPool<Object_t, PoolSize_v>::SetAvailable(Object_t* obj)
{
	Result<uint16_t, Error> f = Find(obj);
	if(f.has_value())
	{
		m_inUse[f.value()] = false;
	}
}

template<
	class    Object_t,
	uint16_t PoolSize_v
> Result<uint16_t, Error> ObjectPool<Object_t, PoolSize_v>::Find(Object_t* obj)
{
	for(uint16_t i = 0; i < PoolSize_v; ++i)
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