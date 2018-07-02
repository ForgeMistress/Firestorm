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
: m_running(true)
, m_paused(false)
{
}

Engine::~Engine()
{
}

void Engine::Update(double deltaT)
{
	if(m_running)
	{
		if(!m_paused)
		{
			ManageSystems();
			ManageEntities();

			
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
	if(std::find(m_entitiesToChange.begin(), m_entitiesToChange.end(), entity) == m_entitiesToChange.end())
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

void Engine::ManageSystems()
{
	if(m_systemsToAdd.empty() && m_systemsToRemove.empty())
	{
		return;
	}

	for(auto system : m_systemsToRemove)
	{
		assert(system);
		for(auto entity : system->m_entities)
		{
			system->OnEntityRemoved(entity);
		}
	}
}

void Engine::ManageEntities()
{

}

CLOSE_NAMESPACE(Elf);