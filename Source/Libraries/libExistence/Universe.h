///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Universe
//
//  It is everything and it is nothing. It is, the Universe.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Universe 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_UNIVERSE_H_
#define LIBEXISTENCE_UNIVERSE_H_
#pragma once

#include "System.h"
#include "Entity.h"

OPEN_NAMESPACE(Firestorm);

class Universe final
{
public:
private:
};

/**
	\class Engine
	\brief Holds Systems and Entities.

	Engine is the class that brings Systems and Entities together. It holds ownership of the
	Systems and Entities that are passed to it through #Engine::AddSystem and #Engine::AddEntity.
 **/
/*class Engine final : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(Engine, Mirror::Object);
public:
	Engine();
	virtual ~Engine();

	void Pause();
	void Unpause();
	void TogglePause();
	void ShutDown();

	void Update(double deltaT);

	template <class T>
	bool AddSystem()
	{
		return AddSystem(std::make_shared<T>());
	}
	bool AddSystem(Mirror::Type systemType);

	template <class T>
	bool RemoveSystem()
	{
		return RemoveSystem(T::MyType());
	}
	bool RemoveSystem(Mirror::Type type);

	bool AddEntity(const RefPtr<Entity>& entity);
	bool RemoveEntity(const WeakPtr<Entity>& entity);

	void Refresh();

	const String& GetName() const;
	void SetName(const String& name);

	size_t GetNumSystems() const;
	WeakPtr<System> GetSystem(size_t index) const;

	template <class T>
	Vector<WeakPtr<System>> GetSystems() const
	{
		return GetSystems(T::MyType());
	}
	Vector<WeakPtr<System>> GetSystems(Mirror::Type type) const;

	size_t GetNumEntities() const;
	WeakPtr<Entity> GetEntity(size_t index) const;

private:
	friend class Entity;
	void ModifyEntity(Entity* entity);

	void ManageSystems();
	void ManageEntities();

	bool RemoveSystem(const RefPtr<System>& system);
	bool AddSystem(const RefPtr<System>& system);

	typedef Vector<RefPtr<System>> SystemList;
	typedef Vector<RefPtr<Entity>> EntityList;

	// Reflected
	String     _name;
	SystemList _systems;
	EntityList _entities;

	UnorderedSet<Mirror::Type> _systemTypes;

	// Runtime
	bool _running;
	bool _paused;

	using EntityPtr = RefPtr<Entity>;
	using SystemPtr = RefPtr<System>;

	UnorderedSet<EntityPtr> _entitiesToAdd;
	UnorderedSet<EntityPtr> _entitiesToModify;
	UnorderedSet<EntityPtr> _entitiesToRemove;

	UnorderedSet<SystemPtr> _systemsToAdd;
	UnorderedSet<SystemPtr> _systemsToRemove;
};*/

CLOSE_NAMESPACE(Firestorm);
#endif
