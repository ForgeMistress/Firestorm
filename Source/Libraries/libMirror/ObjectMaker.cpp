///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ObjectMaker
//
//  Implements a factory for objects.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ObjectMaker.h"

OPEN_NAMESPACE(Firestorm);

ObjectMaker::ObjectMaker()
{
}

ObjectMaker::~ObjectMaker()
{
	std::scoped_lock lock(_s_allLock);
	for(auto makerPair : _makers)
	{
		delete makerPair.second;
	}
	_makers.clear();
}

bool ObjectMaker::RegisterMaker(FireClassID type, IMaker* maker)
{
	std::scoped_lock lock(_s_allLock);
	if(_makers.find(type) == _makers.end())
	{
		_makers[type] = maker;
		return true;
	}
	return false;
}

void* ObjectMaker::Make(FireClassID type)
{
	IMaker* maker = GetMaker(type);
	return maker->Make();
}

bool ObjectMaker::IsMakerRegistered(FireClassID type) const
{
	return _makers.find(type) != _makers.end();
}

IMaker* ObjectMaker::GetMaker(FireClassID type) const
{
	std::scoped_lock<std::mutex> lock(_s_allLock);
	auto found = _makers.find(type);
	if(found != _makers.end())
		return (*found).second;
	return nullptr;
}

CLOSE_NAMESPACE(Firestorm);