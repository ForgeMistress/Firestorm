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

#include <libMirror/Object.h>

OPEN_NAMESPACE(Elf);

RTTR_REGISTRATION
{
	MIRROR_DEFINE(Elf::System);
}

System::System()
: m_modified(false)
{
}

System::~System()
{
}

void System::AddToEngine(SharedPtr<Engine>& engine)
{
	// assert(m_engine.get() == nullptr && "System is already part of an Engine instance.");
	if(is_uninitialized(m_engine))
	{
		m_engine = engine;
		OnAddToEngine();
	}
}

bool System::Filter(const SharedPtr<Entity>& entity) const
{
	return OnEntityFilter(entity);
}

const WeakPtr<Engine>& System::GetEngine() const
{
	return m_engine;
}

const String& System::GetName() const
{
	return m_name;
}

bool System::Contains(const SharedPtr<Entity>& entity) const
{
	return std::find(m_entities.begin(), m_entities.end(), m_entities) != m_entities.end();
}

bool System::AddEntity(SharedPtr<Entity>& entity)
{
	if (OnEntityFilter(entity))
	{
		m_entities.push_back(entity);
		return true;
	}
	return false;
}

void* System::DoInspect(Mirror::Type type)
{
	DOINSPECT_SIMPLE(type);
	return IInspectableObject::DoInspect(type);
}

CLOSE_NAMESPACE(Elf);