///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ScriptComponent.cpp
//
//  A component that contains a reference to a script resource.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ScriptComponent.h"

#include <libMirror/Object.h>

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(Firestorm::ScriptComponent)
{
}

ScriptComponent::ScriptComponent()
{
}

ScriptComponent::~ScriptComponent()
{
}


void* ScriptComponent::DoInspect(Mirror::Type type)
{
	if(ScriptComponent::MyType() == type)
		return static_cast<ScriptComponent*>(this);
	return Component::DoInspect(type);
}

CLOSE_NAMESPACE(Firestorm);