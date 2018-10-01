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

#define ENT_INDEX_BITS      24
#define ENT_INDEX_MASK      ((1 << ENT_INDEX_BITS) - 1)
#define ENT_GENERATION_BITS 8
#define ENT_GENERATION_MASK ((1 << ENT_GENERATION_BITS) - 1)

using ID = uint32_t;

struct Entity final
{
	ID id;

	inline uint32_t Index() const
	{
		return id & ENT_INDEX_MASK;
	}

	inline uint32_t Generation() const
	{
		return (id >> ENT_INDEX_BITS) & ENT_GENERATION_MASK;
	}
};


class EntityMgr final
{
public:
	Entity Create();

	bool IsAlive(const Entity& entity);

	void Destroy(const Entity& entity);
private:
	uint8_t _generation
};

/*
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
};*/

CLOSE_NAMESPACE(Firestorm);
#endif
