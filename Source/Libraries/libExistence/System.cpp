///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  System.cpp
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "System.h"
#include "Engine.h"

#include <libMirror/Object.h>

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(SystemEvent)
{
}

SystemEvent::SystemEvent(SystemEvent::Type type, WeakPtr<Engine> engine)
: type(type)
, engine(engine)
, entity(nullptr)
{

}
SystemEvent::SystemEvent(Type type, WeakPtr<Entity> entity)
: type(type)
, engine(nullptr)
, entity(entity)
{

}

FIRE_MIRROR_DEFINE(Firestorm::System)
{
	Property("name", &System::GetName, &System::SetName)
	(
		MIRROR_META_SAVELOAD
	);
}

System::System()
: _modified(false)
, _active(false)
, _paused(false)
{
}

// protected
System::~System()
{
	_engine = nullptr;
	_entities.clear();
}

bool System::Filter(const WeakPtr<Entity>& entity) const
{
	if(entity)
	{
		return OnEntityFilter(entity);
	}
	return false;
}

const Engine* System::GetEngine() const
{
	return _engine;
}

const String& System::GetName() const
{
	return _name;
}

void System::SetName(const String& name)
{
	_name = name;
}

bool System::Contains(const WeakPtr<Entity>& entity) const
{
	if(entity)
	{
		return std::find_if(_entities.begin(), _entities.end(), [&entity](const WeakPtr<Entity>& e) {
			return e.Lock().Get() == entity.Lock().Get();
		}) != _entities.end();
	}
	return false;
}

void System::Pause()
{
	_paused = true;
}

bool System::AddEntity(WeakPtr<Entity>& entity)
{
	if(entity)
	{
		if(OnEntityFilter(entity))
		{
			_entities.push_back(entity);
			return true;
		}
	}
	return false;
}

void* System::DoInspect(Mirror::Type type)
{
	DOINSPECT_SIMPLE(type);
	return IInspectableObject::DoInspect(type);
}

void System::AddToEngine(Engine* engine)
{
	if(_engine == nullptr)
	{
		_engine = engine;
		Dispatcher.Dispatch(SystemEvent {
			SystemEvent::kEvent_OnAddedToEngine,
			WeakPtr<Engine>(engine)
		});
		OnAddToEngine();
	}
}

void System::Reset()
{
	_engine = nullptr;
	_entities.clear();
}

CLOSE_NAMESPACE(Firestorm);