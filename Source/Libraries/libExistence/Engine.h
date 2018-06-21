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

	void Update(double deltaT);

	bool AddSystem(SharedPtr<System>& obj);
	bool RemoveSystem(SharedPtr<System>& system);

	bool AddEntity(SharedPtr<Entity>& obj);
	bool RemoveEntity(SharedPtr<Entity>& entity);

	template<class System_t>
	bool AddSystemCategory();

	bool AddSystemCategory(Mirror::Type systemInterfaceType);

private:
	// Reflected
	Vector< SharedPtr<System> > m_systems;
	Vector< SharedPtr<Entity> > m_entities;

	// Runtime
	UnorderedMap< Mirror::Type, Vector<SharedPtr<System> > > m_systemsByType;

	Vector< SharedPtr<System> > m_systemsToAdd;
	Vector< SharedPtr<System> > m_systemsToRemove;
	Vector< SharedPtr<Entity> > m_entitiesToAdd;
	Vector< SharedPtr<Entity> > m_entitiesToRemove;
};

CLOSE_NAMESPACE(Elf);
#endif