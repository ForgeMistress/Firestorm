///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ScriptComponent.h
//
//  A component that contains a reference to a script resource.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCRIPT_SCRIPTCOMPONENT_H_
#define LIBSCRIPT_SCRIPTCOMPONENT_H_
#pragma once

OPEN_NAMESPACE(Elf);

class Entity;

/**
	\class ScriptComponent

	Contains a reference to a script resource.
**/
class ScriptComponent : public Component
{
	ELF_MIRROR_DECLARE(ScriptComponent, Component);
public:
	ScriptComponent();
	virtual ~ScriptComponent();

	String ResourceFile;

protected:
	virtual void* DoInspect(Mirror::Type type);
};

CLOSE_NAMESPACE(Elf);
#endif