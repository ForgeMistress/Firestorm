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

OPEN_NAMESPACE(Elf);

RTTR_REGISTRATION
{
	MIRROR_DEFINE(Elf::Component);
}

Component::Component()
{
}

Component::~Component()
{
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
	DOINSPECT_SIMPLE(type);
	return IInspectableObject::DoInspect(type);
}


CLOSE_NAMESPACE(Elf);