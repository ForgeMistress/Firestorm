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

OPEN_NAMESPACE(Firestorm);

class Component;

class Entity : public Mirror::Object,
               public Mirror::IInspectableObject
{
	FIRE_MIRROR_DECLARE(Firestorm::Entity, Firestorm::Mirror::Object, Firestorm::Mirror::IInspectableObject);
public:
	Entity();
	virtual ~Entity();

	inline const String& GetName() const { return _name; }

private:
	virtual void* DoInspect(Mirror::Type type);

	// Reflected
	String                    _name;
	Vector<RefPtr<Component>> _components;

	// Not reflected.
	friend class Engine;
	WeakPtr<Engine> _owningEngine;
};

CLOSE_NAMESPACE(Firestorm);
#endif