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
	virtual void* Make() = 0;
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

	template<class T, class... Args_t>
	T* Make()
	{
		return reinterpret_cast<T*>(Make(T::MyType()));
	}
	void* Make(Mirror::Type type);

	template<class T>
	bool IsMakerRegistered()
	{
		return IsMakerRegistered(T::MyType());
	}
	bool IsMakerRegistered(Mirror::Type type) const;

private:
	IMaker* GetMaker(Mirror::Type type) const;
	UnorderedMap<Mirror::Type, IMaker*> _makers;
	mutable Mutex _lock;
};

CLOSE_NAMESPACE(Firestorm);

#endif

