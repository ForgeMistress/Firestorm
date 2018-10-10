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

struct Buffer
{
	template<class T>
	static T* New(size_t initialSize)
	{
		void* ptr = libCore::Alloc(sizeof(BufferInfo) + (sizeof(T) * initialSize));
		return static_cast<T*>((char*)ptr + sizeof(BufferInfo));
	}

	static void Delete(void* buffer)
	{
		libCore::Free(FIRE_BUFFER_HEADER(buffer));
	}
};

#endif