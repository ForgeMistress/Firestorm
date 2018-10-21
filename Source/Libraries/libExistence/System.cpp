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
#include "Universe.h"

#include <libCore/Object.h>

OPEN_NAMESPACE(Firestorm);

/*FIRE_MIRROR_DEFINE(SystemEvent)
{
}

SystemEvent::SystemEvent(SystemEvent::Type type, Engine* engine)
: type(type)
, engine(engine)
, entity(nullptr)
{

}
SystemEvent::SystemEvent(Type type, Entity* entity)
: type(type)
, engine(nullptr)
, entity(entity)
{

}

FIRE_MIRROR_DEFINE(Firestorm::System)
{
	Class.property("name", &System::GetName, &System::SetName)
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
	if(!entity.expired())
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

void System::Pause()
{
	_paused = true;
}

void* System::DoInspect(Mirror::Type type)
{
	if(type == System::MyType())
	{
		return static_cast<System*>(this);
	}

	return IInspectableObject::DoInspect(type);
}

void System::AddToEngine(Engine* engine)
{
	if(_engine == nullptr)
	{
		_engine = engine;
		Dispatcher.Dispatch(SystemEvent {
			SystemEvent::kEvent_OnAddedToEngine,
			engine
		});
		OnAddToEngine();
	}
}

void System::Reset()
{
	_engine = nullptr;
	_entities.clear();
}*/

CLOSE_NAMESPACE(Firestorm);
