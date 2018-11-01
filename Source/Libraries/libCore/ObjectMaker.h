///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ObjectMaker
//
//  Implements a factory for objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_OBJECTMAKER_H_
#define LIBMIRROR_OBJECTMAKER_H_
#pragma once

#include "MirrorMacros.h"
#include "Object.h"

OPEN_NAMESPACE(Firestorm);

struct IMaker
{
};

template<class T>
struct Maker : public IMaker
{
	eastl::function<eastl::shared_ptr<T>()> _makeFunc;
	template<class F>
	Maker(F&& func)
	: _makeFunc(eastl::forward<F>(func))
	{}

	eastl::shared_ptr<T> Make()
	{
		return eastl::reinterpret_pointer_cast<T>(_makeFunc());
	}
};

class ObjectMaker final
{
public:
	ObjectMaker();
	~ObjectMaker();

	template<class Type>
	bool RegisterMaker(IMaker* maker)
	{
		FIRE_ASSERT(maker != nullptr);
		return RegisterMaker(Type::MyType(), maker);
	}
	bool RegisterMaker(FireClassID baseType, IMaker* maker);

	template<class T, class... Args>
	eastl::shared_ptr<T> Make(Args&&... args)
	{
		auto type = T::MyType();
		IMaker* maker = GetMaker(type);
		Maker<T>* castedMaker = reinterpret_cast<Maker<T>*>(maker);
		return castedMaker->Make();
	}

	template<class T>
	bool IsMakerRegistered()
	{
		return IsMakerRegistered(T::MyType());
	}
	bool IsMakerRegistered(FireClassID type) const;

private:
	IMaker* GetMaker(FireClassID type) const;

	unordered_map<FireClassID, IMaker*> _makers;

	mutable std::mutex _allLock;
};

CLOSE_NAMESPACE(Firestorm);


#endif

