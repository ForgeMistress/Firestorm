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

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);

#define ENT_INDEX_BITS      24
#define ENT_INDEX_MASK      ((1 << ENT_INDEX_BITS) - 1)
#define ENT_GENERATION_BITS 8
#define ENT_GENERATION_MASK ((1 << ENT_GENERATION_BITS) - 1)

#define FIRE_MIN_FREE_INDICES 1024


using EntityID = ID;

/*class Entity final
{
public:
	Entity(uint32_t idx, uint32_t generation)
	: _id((generation << ENT_INDEX_BITS) | idx)
	{
	}

	inline uint32_t ID() const
	{
		return _id;
	}

	inline operator String() const
	{
		return Format("Entity[ IDX:%d, GEN:%d ]", Index(), Generation());
	}

	static const Entity dummy;

private:
	ID _id;
};


class EntityMgr final
{
public:
		Create a brand new Entity.
	Entity Create();

		Check whether or not the Entity is alive.
	bool IsAlive(const Entity& entity);

		Release the Entity.
	void Destroy(const Entity& entity);
private:
	Vector<uint8_t>      _generations;
	std::deque<uint32_t> _freeIndices;
};

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
