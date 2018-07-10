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
		m_entitiesToChange.push_back(entity);
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
	if(m_entitiesToRemove.empty() && m_entitiesToChange.empty())
	{
		return;
	}

	// change entities
	for(auto entity : m_entitiesToChange)
	{
		// add if needed.
		if(std::find(m_entities.begin(), m_entities.end(), entity) == m_entities.end())
		{
			m_entities.push_back(entity);
		}

		for(auto system : m_systems)
		{
			//bool contains = system->Contains(entity);
			Vector<WeakPtr<Entity> >& entities = system->m_entities;
			auto found = std::find(entities.begin(), entities.end(), entity);
			if(system->Filter(entity))
			{
				if(found == entities.end())
				{
					system->m_modified = true;
					entities.push_back(entity);
					system->OnEntityAdded(entity);
				}
			}
			else if(found != entities.end())
			{
				system->m_modified = true;
				entities.erase(found);
				system->OnEntityRemoved(entity);
			}
		}
	}

	// remove entities
	for(auto entity : m_entitiesToRemove)
	{
		auto entityItr = std::find(m_entities.begin(), m_entities.end(), entity);
		if(entityItr != m_entities.end())
		{
			//m_entities.erase(entityItr);
			for(auto system : m_systems)
			{
				Vector<WeakPtr<Entity> >& entities = system->m_entities;
				auto systemContainsItr = std::find(entities.begin(), entities.end(), entity);
				if(systemContainsItr != entities.end())
				{
					system->m_modified = true;
					entities.erase(systemContainsItr);
					system->OnEntityRemoved(entity);
				}
			}
		}
	}

	m_entitiesToRemove.clear();
	m_entitiesToChange.clear();
}

CLOSE_NAMESPACE(Elf);