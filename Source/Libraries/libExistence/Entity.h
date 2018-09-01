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
	ELF_MIRROR_DECLARE(Elf::Entity, Elf::Mirror::Object, Elf::Mirror::IInspectableObject);
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
typedef RefPtr<Entity> EntityPtr;
typedef WeakPtr<Entity>   EntityWeakPtr;

CLOSE_NAMESPACE(Elf);
#endif