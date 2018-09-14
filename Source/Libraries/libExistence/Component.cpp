///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Component.cpp
//
//  A component in an entity that contains only data at the high level.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Component.h"

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(Firestorm::Component)
{
	Property("name", &Component::GetName, &Component::SetName);
}

Component::Component()
{
}

Component::~Component()
{
}

const String& Component::GetName() const
{
	return m_name;
}

void Component::SetName(const String& name)
{
	m_name = name;
}

const WeakPtr<Entity>& Component::GetEntity() const
{
	return m_entity;
}

WeakPtr<Entity>& Component::GetEntity()
{
	return m_entity;
}

void* Component::DoInspect(Mirror::Type type)
{
	if(Component::MyType() == type)
		return static_cast<Component*>(this);
	return IInspectableObject::DoInspect(type);
}

CLOSE_NAMESPACE(Firestorm);