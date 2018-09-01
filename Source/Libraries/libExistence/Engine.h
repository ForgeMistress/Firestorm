///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Engine.h
//
//  An object that holds a number of Systems and Entities.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_ENGINE_H_
#define LIBEXISTENCE_ENGINE_H_
#pragma once

#include <libCore/RefPtr.h>
#include <libMirror/Object.h>

OPEN_NAMESPACE(Elf);

class System;
class Entity;

/**
	\class Engine
	\brief Holds Systems and Entities.

	Engine is the class that brings Systems and Entities together. It holds ownership of the 
	\ref Systems and \ref Entities that are passed to it through \ref AddSystem and \ref AddEntity.
 **/
class Engine final : public Mirror::Object
{
	ELF_MIRROR_DECLARE(Engine, Mirror::Object);
public:
	Engine();
	virtual ~Engine();

	inline void Pause();
	inline void Unpause();
	inline void TogglePause();
	inline void ShutDown();

	void Update(double deltaT);

	bool AddSystem(const RefPtr<System>& system);
	bool RemoveSystem(const RefPtr<System>& system);

	bool AddEntity(const RefPtr<Entity>& entity);
	bool RemoveEntity(const RefPtr<Entity>& entity);

	void Refresh();

	const String& GetName() const;
	void SetName(const String& name);

	bool Contains(const WeakPtr<Entity>& entity);

	size_t GetNumSystems() const;
	size_t GetNumEntities() const;

private:
	void ManageSystems();
	void ManageEntities();

	typedef Vector<RefPtr<System> > SystemList;
	typedef Vector<RefPtr<Entity> > EntityList;

	// Reflected
	String     m_name;
	SystemList m_systems;
	EntityList m_entities;

	// Runtime
	bool m_running;
	bool m_paused;

	List< RefPtr<Entity> > m_entitiesToRemove;
	List< RefPtr<Entity> > m_entitiesToChange;
	List< RefPtr<System> > m_systemsToAdd;
	List< RefPtr<System> > m_systemsToRemove;
};

typedef RefPtr<Engine> EnginePtr;
typedef WeakPtr<Engine>   EngineWeakPtr;

CLOSE_NAMESPACE(Elf);
#endif