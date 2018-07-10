///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Engine.h
//
//  An object that holds a number of Systems.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_ENGINE_H_
#define LIBEXISTENCE_ENGINE_H_
#pragma once

OPEN_NAMESPACE(Elf);

class System;
class Entity;

/**
	\class Engine
	\brief Holds Systems and Entities.

	Engine is the class that brings Systems and Entities together. It holds ownership of the Systems
	and Entities that are passed to it through AddSystem and AddEntity.
 **/
class Engine : public Mirror::Object
{
	MIRROR_DECLARE(Engine, Mirror::Object);
public:
	Engine();
	virtual ~Engine();

	inline void Pause()       { m_paused = true; }
	inline void Unpause()     { m_paused = false; }
	inline void TogglePause() { m_paused = !m_paused; }
	inline void ShutDown()    { m_running = false; }

	void Update(double deltaT);

	bool AddSystem(SharedPtr<System>& obj);
	bool RemoveSystem(SharedPtr<System>& system);

	bool AddEntity(SharedPtr<Entity>& obj);
	bool RemoveEntity(SharedPtr<Entity>& entity);

	template<class System_t>
	bool AddSystemCategory();

	bool AddSystemCategory(Mirror::Type systemInterfaceType);

	const String& GetName() const { return m_name; }

	inline bool Contains(const WeakPtr<Entity>& entity);

private:
	void ManageSystems();
	void ManageEntities();

	// Reflected
	String                      m_name;
	Vector< SharedPtr<System> > m_systems;
	Vector< SharedPtr<Entity> > m_entities;

	// Runtime
	bool m_running;
	bool m_paused;
	UnorderedMap< Mirror::Type, Vector<SharedPtr<System> > > m_systemsByType;

	List< SharedPtr<Entity> > m_entitiesToRemove;
	List< SharedPtr<Entity> > m_entitiesToChange;
	List< SharedPtr<System> > m_systemsToAdd;
	List< SharedPtr<System> > m_systemsToRemove;
};

bool Engine::Contains(const WeakPtr<Entity>& entity)
{
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}

CLOSE_NAMESPACE(Elf);
#endif