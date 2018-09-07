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
#include <libCore/RefPtr.h>

OPEN_NAMESPACE(Firestorm);

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
};

CLOSE_NAMESPACE(Firestorm);
#endif