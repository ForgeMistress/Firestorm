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

ELF_MIRROR_REGISTRATION
{
	ELF_MIRROR_DEFINE(Elf::Engine);
}

Engine::Engine()
: m_running(true)
, m_paused(false)
{
}

Engine::~Engine()
{
}

void Engine::Pause()
{
	m_paused = true;
}

void Engine::Unpause()
{
	m_paused = false;
}

void Engine::TogglePause()
{
	m_paused = !m_paused;
}

void Engine::ShutDown()
{
	m_running = false;
}

void Engine::Update(double deltaT)
{
	if(m_running)
	{
		if(!m_paused)
		{
			ManageSystems();
			ManageEntities();

			// iterate the systems backwards and call modification function if they've been modified.
			for(size_t i = m_systems.size() - 1; i >= 0; --i)
			{
				auto system = m_systems[i];
				if (system->m_active)
				{
					if(system->m_modified)
					{
						system->OnModified(deltaT);
					}
					// TODO: OnPreWrap?
				}
			}
			// normal update
			for(auto system : m_systems)
			{
				if(system->m_active)
				{
					// TODO: Time buffering and intervals.
					system->OnUpdate(deltaT, system->m_entities);
				}
				system->m_modified = false;
			}

			// TODO: OnPostWrap?
		}
	}
}

bool Engine::AddSystem(const SharedPtr<System>& system)
{
	if(std::find(m_systemsToAdd.begin(), m_systemsToAdd.end(), system) == m_systemsToAdd.end())
	{
		m_systemsToAdd.push_back(system);
		return true;
	}
	return false;
}

bool Engine::RemoveSystem(const SharedPtr<System>& system)
{
	if(std::find(m_systems.begin(), m_systems.end(), system) != m_systems.end())
	{
		m_systemsToRemove.push_back(system);
		return true;
	}
	return false;
}

bool Engine::AddEntity(const SharedPtr<Entity>& entity)
{
	if(std::find(m_entitiesToChange.begin(), m_entitiesToChange.end(), entity) == m_entitiesToChange.end())
	{
		m_entitiesToChange.push_back(entity);
		return true;
	}
	return false;
}

bool Engine::RemoveEntity(const SharedPtr<Entity>& entity)
{
	if(std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end())
	{
		m_entitiesToRemove.push_back(entity);
		return true;
	}
	return false;
}

void Engine::Refresh()
{
	ManageSystems();
	ManageEntities();

	for(size_t i = m_systems.size() - 1; i >= 0; --i)
	{
		auto system = m_systems[i];
		if(system->m_active)
		{
			system->OnModified(0.0);
		}
	}
}

const String& Engine::GetName() const
{ 
	return m_name;
}

void Engine::SetName(const String& name)
{
	m_name = name;
}

bool Engine::Contains(const WeakPtr<Entity>& entity)
{
	return std::find(m_entities.begin(), m_entities.end(), entity.lock()) != m_entities.end();
}

size_t Engine::GetNumSystems() const
{
	return m_systems.size();
}

size_t Engine::GetNumEntities() const
{
	return m_entities.size();
}

void Engine::ManageSystems()
{
	if(m_systemsToAdd.empty() && m_systemsToRemove.empty())
	{
		return;
	}

	// remove systems
	for(auto system : m_systemsToRemove)
	{
		for(auto entity : system->m_entities)
		{
			system->OnEntityRemoved(entity);
		}
		system->OnRemoveFromEngine();
		system->m_engine = nullptr;
	}

	//add systems
	for(auto system : m_systemsToAdd)
	{
		system->OnBeforeAddToEngine();

		system->m_modified = true;
		system->m_active = true;
		system->m_engine = this;

		system->OnAddToEngine();

		// add entities to the systems
		for(auto entity : m_entities)
		{
			if(system->Filter(entity))
			{
				system->m_entities.push_back(entity);
				system->OnEntityAdded(entity);
			}
		}
	}
	m_systemsToAdd.clear();
	m_systemsToRemove.clear();
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
			Vector<WeakPtr<Entity> >& entities = system->m_entities;
			// if the entity is already inside the 
			auto found = std::find_if(entities.begin(), entities.end(), [&entity](const WeakPtr<Entity>& e) {
				return e.lock().get() == entity.get();
			});
			
			if(found == entities.end())
			{
				if(system->Filter(entity))
				{
					system->m_modified = true;
					entities.push_back(entity);
					system->OnEntityAdded(entity);
				}
			}
			else
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
				Vector<EntityWeakPtr>& entities = system->m_entities;
				auto systemContainsItr = std::find_if(entities.begin(), entities.end(), [&entity](const WeakPtr<Entity>& e) {
					return e.lock().get() == entity.get();
				});
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