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
: _running(true)
, _paused(false)
{
}

Engine::~Engine()
{
}

void Engine::Pause()
{
	_paused = true;
}

void Engine::Unpause()
{
	_paused = false;
}

void Engine::TogglePause()
{
	_paused = !_paused;
}

void Engine::ShutDown()
{
	_running = false;
}

void Engine::Update(double deltaT)
{
	if(_running)
	{
		if(!_paused)
		{
			ManageSystems();
			ManageEntities();

			// iterate the and call modification function if they've been modified.
			if(!_systems.empty())
			{
				for(auto system : _systems)
				{
					if(system->_active)
					{
						if(system->_modified)
						{
							system->OnModified(deltaT);
						}
						// TODO: OnPreWrap?
					}
				}
			}
			// normal update
			for(auto system : _systems)
			{
				if(system->_active)
				{
					// TODO: Time buffering and intervals.
					system->OnTick(deltaT, system->_entities);
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
	if(std::find(_systemsToAdd.begin(), _systemsToAdd.end(), system) == _systemsToAdd.end())
	{
		_systemsToAdd.push_back(system);
		return true;
	}
	return false;
}

bool Engine::RemoveSystem(Mirror::Type systemType)
{
	auto findFunction = [&systemType](const RefPtr<System>& system) {
		return system->GetType() == systemType;
	};
	auto found = std::find_if(_systems.begin(), _systems.end(), findFunction);
	if(found != _systems.end())
	{
		_systemsToRemove.push_back((*found));
		return true;
	}
	return false;
}

bool Engine::AddEntity(const RefPtr<Entity>& entity)
{
	if(std::find(_entitiesToAdd.begin(), _entitiesToAdd.end(), entity) == _entitiesToAdd.end())
	{
		_entitiesToAdd.push_back(entity);
		return true;
	}
	return false;
}

bool Engine::RemoveEntity(const RefPtr<Entity>& entity)
{
	if(std::find(_entities.begin(), _entities.end(), entity) != _entities.end())
	{
		_entitiesToRemove.push_back(entity);
		return true;
	}
	return false;
}

void Engine::Refresh()
{
	ManageSystems();
	ManageEntities();

	if(!_systems.empty())
	{
		for(size_t i = _systems.size() - 1; i >= 0; --i)
		{
			auto system = _systems[i];
			if(system->_active)
			{
				system->OnModified(0.0);
			}
		}
	}
}

const String& Engine::GetName() const
{ 
	return _name;
}

void Engine::SetName(const String& name)
{
	_name = name;
}

bool Engine::Contains(const WeakPtr<Entity>& entity)
{
	return std::find(_entities.begin(), _entities.end(), entity.Lock()) != _entities.end();
}

size_t Engine::GetNumSystems() const
{
	return _systems.size();
}

size_t Engine::GetNumEntities() const
{
	return _entities.size();
}

void Engine::ModifyEntity(Entity* entity)
{
	auto findFunction = [&entity](const RefPtr<Entity>& e) {
		return e.Get() == entity;
	};
	auto found = std::find_if(_entities.begin(), _entities.end(), findFunction);
	FIRE_ASSERT(found != _entities.end());

	if(!_entitiesToRemove.empty())
	{
		FIRE_ASSERT(std::find_if(_entitiesToRemove.begin(), _entitiesToRemove.end(), findFunction) == _entitiesToRemove.end());
	}

	if(std::find_if(_entitiesToModify.begin(), _entitiesToModify.end(), findFunction) == _entitiesToModify.end())
	{
		_entitiesToModify.push_back(*found);
	}
}

void Engine::ManageSystems()
{
	if(_systemsToAdd.empty() && _systemsToRemove.empty())
	{
		return;
	}

	for(auto system : _systemsToRemove)
	{
		for(auto entity : system->_entities)
		{
			system->OnEntityRemoved(entity);
		}
		system->OnRemoveFromEngine();
		system->_engine = nullptr;
		std::remove(_systems.begin(), _systems.end(), system);
	}

	for(auto system : _systemsToAdd)
	{
		system->OnBeforeAddToEngine();
		system->_modified = true;
		system->_engine = this;

		_systems.push_back(system);
		system->OnAddToEngine();

		for(auto entity : _entities)
		{
			if(system->Filter(entity))
			{
				system->_entities.push_back(entity);
				system->OnEntityAdded(entity);
			}
		}
	}
	_systemsToAdd.clear();
	_systemsToRemove.clear();
}

void Engine::ManageEntities()
{
	if(_entitiesToRemove.empty() && _entitiesToAdd.empty() && _entitiesToModify.empty())
	{
		return;
	}

	for(auto entity : _entitiesToAdd)
	{
		// add the entity to the engine if needed
		if(std::find(_entities.begin(), _entities.end(), entity) == _entities.end())
		{
			entity->_owningEngine = this;
			_entities.push_back(entity);
		}

		// go through the systems and add entities if it will take them.
		for(auto system : _systems)
		{
			if(system->Filter(entity))
			{
				Vector<WeakPtr<Entity>>& systemEntities = system->_entities;
				auto found = std::find(systemEntities.begin(), systemEntities.end(), entity);
				if (found == systemEntities.end())
				{
					system->_modified = true;
					systemEntities.push_back(entity);
					system->OnEntityAdded(entity);
				}
			}
		}
	}

	// If entities reported that they had new components added to them, we need to add them to apropriate systems.
	for(auto entity : _entitiesToModify)
	{
		for(auto system : _systems)
		{
			Vector<WeakPtr<Entity>>& systemEntities = system->_entities;
			auto found = std::find(systemEntities.begin(), systemEntities.end(), entity);
			if(system->Filter(entity))
			{
				if(found == systemEntities.end())
				{
					system->_modified = true;
					systemEntities.push_back(entity);
					system->OnEntityAdded(entity);
				}
			}
			// if it's no longer valid for this system, we need to remove it.
			else
			{
				if(found != systemEntities.end())
				{
					system->_modified = true;
					systemEntities.erase(found);
					system->OnEntityRemoved(entity);
				}
			}
		}
	}

	// finally, remove all entities that need to be removed
	for(auto entity : _entitiesToRemove)
	{
		auto foundEntity = std::find(_entities.begin(), _entities.end(), entity);
		for(auto system : _systems)
		{
			Vector<WeakPtr<Entity>>& systemEntities = system->_entities;
			auto foundInSystem = std::find(systemEntities.begin(), systemEntities.end(), entity);
			if(foundInSystem != systemEntities.end())
			{
				system->_modified = true;
				systemEntities.erase(foundInSystem);
				system->OnEntityRemoved(entity);
			}
		}
	}

	_entitiesToAdd.clear();
	_entitiesToModify.clear();
	_entitiesToRemove.clear();
}

CLOSE_NAMESPACE(Firestorm);