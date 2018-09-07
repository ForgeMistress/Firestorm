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

OPEN_NAMESPACE(Firestorm);


FIRE_MIRROR_DEFINE(Firestorm::Engine)
{

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
				if (system->_active)
				{
					if(system->_modified)
					{
						system->OnModified(deltaT);
					}
					// TODO: OnPreWrap?
				}
			}
			// normal update
			for(auto system : m_systems)
			{
				if(system->_active)
				{
					// TODO: Time buffering and intervals.
					system->OnUpdate(deltaT, system->_entities);
				}
				system->_modified = false;
			}

			// TODO: OnPostWrap?
		}
	}
}

bool Engine::AddSystem(const String& system)
{
	Mirror::Type systemType = Mirror::Type::get_by_name(system);
	if(systemType.is_valid())
	{
		Mirror::Instance instance = systemType.create();
		if(instance.is_valid())
		{
			return AddSystem(instance.try_convert<System>());
		}
	}
	return false;
}

bool Engine::RemoveSystem(const String& system)
{
	return false;
}

bool Engine::AddSystem(const RefPtr<System>& system)
{
	if(std::find(m_systemsToAdd.begin(), m_systemsToAdd.end(), system) == m_systemsToAdd.end())
	{
		m_systemsToAdd.push_back(system);
		return true;
	}
	return false;
}

bool Engine::RemoveSystem(const RefPtr<System>& system)
{
	if(std::find(m_systems.begin(), m_systems.end(), system) != m_systems.end())
	{
		m_systemsToRemove.push_back(system);
		return true;
	}
	return false;
}

bool Engine::AddEntity(const RefPtr<Entity>& entity)
{
	if(std::find(m_entitiesToChange.begin(), m_entitiesToChange.end(), entity) == m_entitiesToChange.end())
	{
		m_entitiesToChange.push_back(entity);
		return true;
	}
	return false;
}

bool Engine::RemoveEntity(const RefPtr<Entity>& entity)
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
		if(system->_active)
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
	return std::find(m_entities.begin(), m_entities.end(), entity.Lock()) != m_entities.end();
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
		for(auto entity : system->_entities)
		{
			system->OnEntityRemoved(entity);
		}
		system->OnRemoveFromEngine();
		system->_engine = nullptr;
	}

	//add systems
	for(auto system : m_systemsToAdd)
	{
		system->OnBeforeAddToEngine();

		system->_modified = true;
		system->_active = true;
		system->_engine = this;

		system->OnAddToEngine();

		// add entities to the systems
		for(auto entity : m_entities)
		{
			if(system->Filter(entity))
			{
				system->_entities.push_back(entity);
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
			Vector<WeakPtr<Entity> >& entities = system->_entities;
			// if the entity is already inside the 
			auto found = std::find_if(entities.begin(), entities.end(), [&entity](const WeakPtr<Entity>& e) {
				return e.Lock().Get() == entity.Get();
			});
			
			if(found == entities.end())
			{
				if(system->Filter(entity))
				{
					system->_modified = true;
					entities.push_back(entity);
					system->OnEntityAdded(entity);
				}
			}
			else
			{
				system->_modified = true;
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
				Vector<WeakPtr<Entity>>& entities = system->_entities;
				auto systemContainsItr = std::find_if(entities.begin(), entities.end(), [&entity](const WeakPtr<Entity>& e) {
					return e.Lock().Get() == entity.Get();
				});
				if(systemContainsItr != entities.end())
				{
					system->_modified = true;
					entities.erase(systemContainsItr);
					system->OnEntityRemoved(entity);
				}
			}
		}
	}

	m_entitiesToRemove.clear();
	m_entitiesToChange.clear();
}

CLOSE_NAMESPACE(Firestorm);