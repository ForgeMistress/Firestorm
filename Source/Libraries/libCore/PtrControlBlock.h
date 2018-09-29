///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PtrControlBlock
//
//  Control block for RefPtr and WeakPtr.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_PTRCONTROLBLOCK_H_
#define LIBCORE_PTRCONTROLBLOCK_H_
#pragma once

#include "ObjectPool.h"
#include <atomic>

OPEN_NAMESPACE(Firestorm);

#define nullptr_u (uintptr_t)nullptr

class PtrControlBlock final
{
	//template <class T> friend class WeakPtr;
private:
	static ObjectPool<PtrControlBlock> _s_pool;

public:
	template<class T>
	static PtrControlBlock* make_block(T* ptr)
	{
		return new PtrControlBlock(ptr);
	}

	static PtrControlBlock* make_block(std::nullptr_t)
	{
		return new PtrControlBlock(nullptr);
	}

	static void return_block(PtrControlBlock* ctrl)
	{
		delete ctrl;
	}

	template <class T>
	PtrControlBlock(T* object)
	: _object(static_cast<void*>(object))
	, _weakCount(0)
	{
	}

	PtrControlBlock(std::nullptr_t)
	: _object(nullptr)
	, _weakCount(0)
	{
	}

	PtrControlBlock(const PtrControlBlock&) = delete;
	PtrControlBlock(PtrControlBlock&&) = delete;

	template<class T>
	T* get_ptr() const
	{
		return static_cast<T*>(_object);
	}

	template<class T>
	T* get_ptr()
	{
		return static_cast<T*>(_object);
	}

	template<class T>
	void set_ptr(T* ptr)
	{
		_object = ptr;
	}

	void set_ptr(std::nullptr_t)
	{
		_object = nullptr;
	}

	template<class T, class Del>
	void delete_ptr(Del deleter)
	{
		if(_object != nullptr)
		{
			T* obj = get_ptr<T>();
			// then delete the object.
			if(deleter) deleter(obj);
			else delete obj;
		}
	}

	size_t add_weak_ref()
	{
		++_weakCount;
		return _weakCount;
	}

	size_t del_weak_ref()
	{
		FIRE_ASSERT_MSG(_weakCount != 0, "can not del_weak_ref. _weakCount is already 0.");
		--_weakCount;
		return _weakCount;
	}

	size_t get_weak_count() const
	{
		return _weakCount;
	}

	size_t add_strong_ref()
	{
		++_strongCount;
		return _strongCount;
	}

	size_t del_strong_ref()
	{
		FIRE_ASSERT_MSG(_strongCount != 0, "can not del_strong_ref. _strongCount is already 0.");
		--_strongCount;
		return _strongCount;
	}

	size_t get_strong_count() const
	{
		return _strongCount;
	}

private:
	void*               _object{ nullptr };
	std::atomic<size_t> _strongCount{ 0 };
	std::atomic<size_t> _weakCount{ 0 };
};

CLOSE_NAMESPACE(Firestorm);
#endif