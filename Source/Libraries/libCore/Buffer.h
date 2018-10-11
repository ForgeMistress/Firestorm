///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Buffer
//
//  A stretchy buffer.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_BUFFER_H_
#define LIBCORE_BUFFER_H_
#pragma once

#include <libCore/libCore.h>

#define FIRE_BUFFER_HEADER(ptr) \
	((BufferInfo*)((char*)(ptr) - sizeof(BufferInfo)))

#define FIRE_BUFFER_SIZE(ptr) \
	((ptr) ? FIRE_BUFFER_HEADER(ptr)->size : 0)

struct BufferInfo
{
	size_t size;
	size_t capacity;
};

template<class T>
struct Buffer
{
	template<class T>
	static T* New(size_t initialSize)
	{
		void* ptr = fire_alloc(sizeof(BufferInfo) + (sizeof(T) * initialSize));
		return static_cast<T*>((char*)ptr + sizeof(BufferInfo));
	}

	static void Delete(void* buffer)
	{
		fire_free(FIRE_BUFFER_HEADER(buffer));
	}

	void Reserve(size_t newCapacity)
	{
		if(newCapacity < _capacity) { return; } // no-op
		T* oldPtr = _ptr;
		_capacity = newCapacity;
		size_t bufferSize = sizeof(T) * newCapacity;
		_ptr = (T*)fire_alloc(bufferSize);
		if(oldPtr)
		{
			memcpy(_ptr, oldPtr, _size * sizeof(T));
			fire_free(oldPtr);
		}
	}

	void PushBack(const T& value)
	{
		if(_size == _capacity-1)
		{
			Reserve(_capacity * 2);
		}
		_ptr[_size-1] = 
	}


	const T& operator[](size_t index) const
	{
		FIRE_ASSERT(index < _size);
		return _ptr[index];
	}

	T& operator[](size_t index)
	{
		FIRE_ASSERT(index < _size);
		return _ptr[index];
	}

	size_t Size() const
	{
		return _size;
	}

	size_t Capacity() const
	{
		return _capacity;
	}

private:
	void EnsureBufferAllocated(size_t capacity)
	{
		if(!_ptr)
		{
			_ptr = fire_alloc(sizeof(T) * capacity);
		}
	}
	size_t _size{ 0 };
	size_t _capacity{ 0 };
	T* _ptr;
};

#endif