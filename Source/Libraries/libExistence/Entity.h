///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Entity.h
//
//  A collection of components.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_ENTITY_H_
#define LIBEXISTENCE_ENTITY_H_
#pragma once

OPEN_NAMESPACE(Elf);

class Component;

class Entity : public Mirror::Object,
               public Mirror::IInspectableObject
{
public:
	Entity();
	virtual ~Entity();

	inline const String& GetName() const { return m_name; }

private:
	virtual void* DoInspect(Mirror::Type type);

	// Reflected
	String                         m_name;
	Vector< SharedPtr<Component> > m_components;

	// Not reflected.
	friend class Engine;
	WeakPtr< Engine > m_owningEngine;
};
typedef SharedPtr<Entity> EntityPtr;
typedef WeakPtr<Entity>   EntityWeakPtr;

CLOSE_NAMESPACE(Elf);
#endif