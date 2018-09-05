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

ELF_MIRROR_DEFINE(Elf::System)
{
	_class.property("name", &System::GetName, &System::SetName)
	(
		MIRROR_META_SAVELOAD
	);
}

System::System()
: m_modified(false)
, m_active(false)
, m_paused(false)
{
}

// protected
System::~System()
{
	m_engine = nullptr;
	m_entities.clear();
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
	return m_engine;
}

const String& System::GetName() const
{
	return m_name;
}

void System::SetName(const String& name)
{
	m_name = name;
}

bool System::Contains(const WeakPtr<Entity>& entity) const
{
	if(entity)
	{
		return std::find_if(m_entities.begin(), m_entities.end(), [&entity](const WeakPtr<Entity>& e) {
			return e.Lock().Get() == entity.Lock().Get();
		}) != m_entities.end();
	}
	return false;
}

void System::Pause()
{
	m_paused = true;
}

bool System::AddEntity(WeakPtr<Entity>& entity)
{
	if(entity)
	{
		if(OnEntityFilter(entity))
		{
			m_entities.push_back(entity);
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
	if(m_engine == nullptr)
	{
		m_engine = engine;
		OnAddToEngine();
	}
}

void System::Reset()
{
	m_engine = nullptr;
	m_entities.clear();
}

CLOSE_NAMESPACE(Elf);