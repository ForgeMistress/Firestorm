///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ObjectMaker
//
//  Implements a factory for objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_OBJECTMAKER_H_
#define LIBMIRROR_OBJECTMAKER_H_
#pragma once

#include "MirrorMacros.h"
#include "Object.h"

OPEN_NAMESPACE(Firestorm);

struct IMaker
{
	virtual void* Make() const = 0;
	virtual void* MakeInPlace(void* place) const = 0;
};

/**
	\class BasicMaker

	A standard no-frills object maker.
 **/
template <class T>
struct BasicMaker : public IMaker
{
	virtual void* Make() const
	{
		return new T();
	}

	virtual void* MakeInPlace(void* place) const
	{
		return new (place) T();
	}
};

class ObjectMaker final
{
public:
	ObjectMaker();
	~ObjectMaker();

	template<class T>
	bool RegisterMaker(IMaker* maker)
	{
		return RegisterMaker(T::MyType(), maker);
	}
	bool RegisterMaker(Mirror::Type type, IMaker* maker);

	template<class T>
	T* Make() const
	{
		return reinterpret_cast<T*>(Make(T::MyType()));
	}
	void* Make(Mirror::Type type) const;

	template<class T>
	T* Make(void* place) const
	{
		return reinterpret_cast<T*>(Make(T::MyType(), place));
	}
	void* Make(Mirror::Type type, void* place) const;

private:
	const IMaker* GetMaker(Mirror::Type type) const;
	Vector<Pair<Mirror::Type, IMaker*>> _makers;
	Mutex _lock;
};

CLOSE_NAMESPACE(Firestorm);

#endif

