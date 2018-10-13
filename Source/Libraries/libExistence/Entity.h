///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EntityMgr
//
//  Creates and destroys entities.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_ENTITY_H_
#define LIBEXISTENCE_ENTITY_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/UUIDMgr.h>

OPEN_NAMESPACE(Firestorm);

using EntityID = uint64_t;
using EntityGeneration = uint16_t;
// since there's no uint48_t type, we'll leave that alone.

// 16 bits for the generation and 48 bits for the index.
static const unsigned ENT_INDEX_BITS = sizeof(EntityID) - sizeof(uint8_t);
static const unsigned ENT_INDEX_MASK = (1<<ENT_INDEX_BITS) - 1;
static const unsigned ENT_GENERATION_BITS = sizeof(EntityID) - ENT_INDEX_BITS;
static const unsigned ENT_GENERATION_MASK = (1<<ENT_GENERATION_BITS) - 1;

static const EntityID ENT_INVALID = eastl::numeric_limits<EntityID>::max();

struct Entity
{
	EntityID id;
	Entity():id(ENT_INVALID) {}
	Entity(EntityID index, uint8_t generation)
	: id((generation << ENT_INDEX_BITS) | index)
	{
	}

	EntityID Index() const { return id & ENT_INDEX_MASK; }
	EntityID Generation() const { return (id >> ENT_INDEX_BITS) & ENT_GENERATION_MASK; }

	inline bool operator==(const Entity& other)
	{
		return other.id == id;
	}

private:
	friend class EntityMgr;
};

struct EntityData
{
};

class EntityMgr final
{
public:
	using DestructionCallback = function<void(Entity)>;

	EntityMgr(UUIDMgr& uuidMgr);

	/**
		Spawn yourself a shiny new entity.
	 **/
	Entity SpawnEntity(EntityData* data = nullptr);

	/**
		Despawn an entity and mark it as dead.
	 **/
	void DespawnEntity(Entity entity);

	/**
		Register a function to be called when an Entity is destroyed.
	 **/
	void RegisterDestructionCallback(void* definition, DestructionCallback callback);

	/**
		Unregister a registrant from the destruction listeners.
	 **/
	void UnregisterDestructionCallback(void* registrant);

	/**
		Check whether or not the Entity is alive.
	 **/
	bool IsAlive(Entity entity);

	/**
		Retrieve the number of registered destructor callbacks.

		\note This is used mostly for unit testing.
	 **/
	size_t GetNumRegisteredDestructors() const { return _destructionCallbacks.size(); }

private:
	void DispatchDestruction(Entity entity);
	void BuildEntity(Entity entity, EntityData* data) const;

	struct CallbackInfo
	{
		DestructionCallback Callback;
		void* Registrant;
	};
	vector<CallbackInfo> _destructionCallbacks;
	vector<uint8_t> _generation;
	deque<EntityID> _freeIndices;
};

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

OPEN_NAMESPACE(eastl);

template<>
struct hash<::Firestorm::Entity>
{
	size_t operator()(const ::Firestorm::Entity& e) const
	{
		size_t index = e.Index();
		return index;
	}
};

static bool operator==(const ::Firestorm::Entity& e1, const ::Firestorm::Entity& e2)
{
	return e1.Index() == e2.Index() &&
		   e1.Generation() == e2.Generation();
}

CLOSE_NAMESPACE(eastl);
#endif
