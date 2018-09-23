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

#include <libCore/IRefCounted.h>

OPEN_NAMESPACE(Firestorm);

class Component;

/**
	An entity's cache coherency can easily be optimized by doing placement new
	with enough space allocated for both the entity _and_ the components we know it is going to hold, leaving room for
	the entity to have components added and removed as we need them. Since the Entity itself is responsible for 
	allocating the components it wants to hold, we are in complete control of the memory at this level.
 **/
class Entity : public Mirror::Object,
               public Mirror::IInspectableObject,
               public IRefCounted
{
	FIRE_MIRROR_DECLARE(Firestorm::Entity, Firestorm::Mirror::Object);
public:
	Entity();
	virtual ~Entity();

	const String& GetName() const { return _name; }

	template <class T>
	bool AddComponent()
	{
		return AddComponent(T::MyType());
	}
	bool AddComponent(const Mirror::Type& type);

	template <class T>
	bool RemoveComponent()
	{
		return RemoveComponent(T::MyType());
	}
	bool RemoveComponent(const String& componentName);
	bool RemoveComponent(Mirror::Type ofType);

	size_t GetNumComponents() const;
	WeakPtr<Component> GetComponent(size_t index) const;
	WeakPtr<Component> GetComponent(const String& name) const;

	template <class T>
	Vector<WeakPtr<Component>> GetComponents() const
	{
		return GetComponents(T::MyType());
	}
	Vector<WeakPtr<Component>> GetComponents(Mirror::Type type) const;

private:
	virtual void* DoInspect(Mirror::Type type);

	bool AddComponent(const RefPtr<Component>& component);
	bool RemoveComponent(const RefPtr<Component>& component);

	bool HasComponentOfType(Mirror::Type type) const;

	// Reflected
	String                     _name;
	Vector<RefPtr<Component>>  _components;

	// Not reflected.
	friend class Engine;
	bool _modified{ false };
	Engine* _owningEngine{ nullptr };
};

CLOSE_NAMESPACE(Firestorm);
#endif