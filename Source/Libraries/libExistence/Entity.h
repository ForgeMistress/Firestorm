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

	bool AddComponent(const RefPtr<Component>& component);

	bool RemoveComponent(const RefPtr<Component>& component);
	bool RemoveComponent(const String& componentName);
	bool RemoveComponent(Mirror::Type ofType);

	size_t GetNumComponents() const;

private:
	virtual void* DoInspect(Mirror::Type type);

	// Reflected
	String                    _name;
	Vector<RefPtr<Component>> _components;

	// Not reflected.
	friend class Engine;
	bool _modified{ false };
	Engine* _owningEngine{ nullptr };
};

CLOSE_NAMESPACE(Firestorm);
#endif