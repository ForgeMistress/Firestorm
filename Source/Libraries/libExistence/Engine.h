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

OPEN_NAMESPACE(Firestorm);

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
	FIRE_MIRROR_DECLARE(Engine, Mirror::Object);
public:
	Engine();
	virtual ~Engine();

	inline void Pause();
	inline void Unpause();
	inline void TogglePause();
	inline void ShutDown();

	void Update(double deltaT);

	bool AddSystem(const String& system);
	bool RemoveSystem(const String& system);

	bool AddSystem(const RefPtr<System>& system);

	template <class T>
	bool RemoveSystem()
	{
		return RemoveSystem(T::MyType());
	}
	bool RemoveSystem(Mirror::Type type);

	bool AddEntity(const RefPtr<Entity>& entity);
	bool RemoveEntity(const RefPtr<Entity>& entity);

	void Refresh();

	const String& GetName() const;
	void SetName(const String& name);

	bool Contains(const WeakPtr<Entity>& entity);

	size_t GetNumSystems() const;
	size_t GetNumEntities() const;

private:
	friend class Entity;
	void ModifyEntity(Entity* entity);

	void ManageSystems();
	void ManageEntities();

	typedef Vector<RefPtr<System>> SystemList;
	typedef Vector<RefPtr<Entity>> EntityList;

	// Reflected
	String     _name;
	SystemList _systems;
	EntityList _entities;

	// Runtime
	bool _running;
	bool _paused;

	List<RefPtr<Entity>> _entitiesToAdd;
	List<RefPtr<Entity>> _entitiesToModify;
	List<RefPtr<Entity>> _entitiesToRemove;
	List<RefPtr<System>> _systemsToAdd;
	List<RefPtr<System>> _systemsToRemove;
};

typedef RefPtr<Engine>  EnginePtr;
typedef WeakPtr<Engine> EngineWeakPtr;

CLOSE_NAMESPACE(Firestorm);
#endif