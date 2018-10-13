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
	bool RegisterMaker(FireClassID type, IMaker* maker);

	template<class T, class... Args_t>
	T* Make()
	{
		return reinterpret_cast<T*>(Make(T::MyType()));
	}
	void* Make(FireClassID type);

	template<class T>
	bool IsMakerRegistered()
	{
		return IsMakerRegistered(T::MyType());
	}
	bool IsMakerRegistered(FireClassID type) const;

private:
	IMaker* GetMaker(FireClassID type) const;
	unordered_map<FireClassID, IMaker*> _makers;
	mutable mutex _s_allLock;
};

CLOSE_NAMESPACE(Firestorm);

#endif

