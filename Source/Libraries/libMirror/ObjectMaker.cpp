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
	std::scoped_lock lock(_lock);
	for(size_t i = 0; i < _makers.size(); ++i)
	{
		delete _makers[i].second;
	}
	_makers.clear();
}

bool ObjectMaker::RegisterMaker(Mirror::Type type, IMaker* maker)
{
	std::scoped_lock lock(_lock);
	for(size_t i = 0; i < _makers.size(); ++i)
	{
		if(_makers[i].first == type)
			return false;
	}
	_makers.push_back(std::make_pair(std::move(type), maker));
	return true;
}

void* ObjectMaker::Make(Mirror::Type type) const
{
	const IMaker* maker = GetMaker(type);
	return maker->Make();
}

void* ObjectMaker::Make(Mirror::Type type, void* place) const
{
	const IMaker* maker = GetMaker(type);
	return maker->MakeInPlace(place);
}

const IMaker* ObjectMaker::GetMaker(Mirror::Type type) const
{
	std::scoped_lock lock(_lock);
	for(size_t i = 0; i < _makers.size(); ++i)
	{
		const auto& item = _makers[i];
		if(item.first == type)
			return item.second;
	}
	return nullptr;
}

CLOSE_NAMESPACE(Firestorm);