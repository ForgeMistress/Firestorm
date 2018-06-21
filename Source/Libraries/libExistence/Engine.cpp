///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Engine.h
//
//  An object that holds a number of Systems.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Engine.h"

#include "System.h"
#include "Entity.h"

OPEN_NAMESPACE(Elf);

RTTR_REGISTRATION
{
	MIRROR_DEFINE(Elf::Engine);
}

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Update(double deltaT)
{
	if(m_systemsToAdd.empty() == false)
	{
		
	}

	// Add entities to systems.
	if(m_entitiesToAdd.empty() == false)
	{
		for(auto entity : m_entitiesToAdd)
		{

		}
	}
}

bool Engine::AddSystem(SharedPtr<System>& system)
{
	if(std::find(m_systemsToAdd.begin(), m_systemsToAdd.end(), system) == m_systemsToAdd.end())
	{
		m_systemsToAdd.push_back(system);
		return true;
	}
	return false;
}

bool Engine::RemoveSystem(SharedPtr<System>& system)
{
	if(std::find(m_systems.begin(), m_systems.end(), system) != m_systems.end())
	{
		m_systemsToRemove.push_back(system);
		return true;
	}
	return false;
}

bool Engine::AddEntity(SharedPtr<Entity>& entity)
{
	if(std::find(m_entitiesToAdd.begin(), m_entitiesToAdd.end(), entity) == m_entitiesToAdd.end())
	{
		m_entitiesToAdd.push_back(entity);
		return true;
	}
	return false;
}

bool Engine::RemoveEntity(SharedPtr<Entity>& entity)
{
	if(std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end())
	{
		m_entitiesToRemove.push_back(entity);
		return true;
	}
	return false;
}

CLOSE_NAMESPACE(Elf);