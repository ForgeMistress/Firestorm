///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Universe
//
//  It is everything and it is nothing. It is, the Universe.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Universe.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*FIRE_MIRROR_DEFINE(Firestorm::Engine)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Engine::Engine()
: _running(true)
, _paused(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Engine::~Engine()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Pause()
{
	_paused = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Unpause()
{
	_paused = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::TogglePause()
{
	_paused = !_paused;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::ShutDown()
{
	_running = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Engine::AddSystem(Mirror::Type systemType)
{
	FIRE_ASSERT(systemType.is_valid());
	if(_systemTypes.find(systemType) == _systemTypes.end())
	{
		Mirror::Instance instance = systemType.create();
		if(instance.is_valid())
		{
			return AddSystem(RefPtr<System>(instance.try_convert<System>()));
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Engine::AddSystem(const RefPtr<System>& system)
{
	if(_systemTypes.find(system->GetType()) == _systemTypes.end())
	{
		if(_systemsToAdd.find(system) == _systemsToAdd.end())
		{
			_systemsToAdd.insert(system);
			_systemTypes.insert(system->GetType());
		}
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Engine::RemoveSystem(Mirror::Type type)
{
	FIRE_ASSERT(type.is_valid());
	if(_systemTypes.find(type) != _systemTypes.end())
	{
		auto found = std::find_if(_systems.begin(), _systems.end(), [&type](const RefPtr<System>& system) {
			return system->GetType() == type;
		});
		if(found != _systems.end())
		{
			_systemsToRemove.insert((*found));
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PRIVATE
bool Engine::RemoveSystem(const RefPtr<System>& system)
{
	auto found = std::find_if(_systems.begin(), _systems.end(), [&system](const RefPtr<System>& sys) {
		return system == sys;
	});
	if(found != _systems.end())
	{
		_systemsToRemove.insert(system);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Engine::AddEntity(const RefPtr<Entity>& entity)
{
	if(Exists(_entitiesToAdd, entity))
	{
		_entitiesToAdd.insert(entity);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// PRIVATE
bool Engine::RemoveEntity(const WeakPtr<Entity>& entity)
{
	if(Exists(_entities, entity.lock()))
	{
		_entitiesToRemove.insert(entity.lock());
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const String& Engine::GetName() const
{
	return _name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::SetName(const String& name)
{
	_name = name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Engine::GetNumSystems() const
{
	return _systems.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WeakPtr<System> Engine::GetSystem(size_t index) const
{
	FIRE_ASSERT(index < _systems.size());
	return _systems[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<WeakPtr<System>> Engine::GetSystems(Mirror::Type type) const
{
	Vector<WeakPtr<System>> output;
	for(auto system : _systems)
	{
		if (system->Inspect(type) != nullptr)
		{
			output.push_back(system);
		}
	}
	return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Engine::GetNumEntities() const
{
	return _entities.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WeakPtr<Entity> Engine::GetEntity(size_t index) const
{
	FIRE_ASSERT(index < _entities.size());
	return _entities[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::ModifyEntity(Entity* entity)
{
	auto findFunction = [&entity](const RefPtr<Entity>& e) {
		return e.get() == entity;
	};
	auto found = std::find_if(_entities.begin(), _entities.end(), findFunction);
	FIRE_ASSERT(found != _entities.end());

	if(!_entitiesToRemove.empty())
	{
		if(std::find_if(_entitiesToRemove.begin(), _entitiesToRemove.end(), findFunction) == _entitiesToRemove.end())
		{
			return;
		}
	}

	if(std::find_if(_entitiesToModify.begin(), _entitiesToModify.end(), findFunction) == _entitiesToModify.end())
	{
		_entitiesToModify.insert(*found);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		Remove(_systems, system);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
				auto found = std::find_if(systemEntities.begin(), systemEntities.end(), [&entity](const WeakPtr<Entity>& e) {
					return e.lock() == entity;
				});
				if(found == systemEntities.end())
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
			auto found = std::find_if(systemEntities.begin(), systemEntities.end(), [&entity](const WeakPtr<Entity>& e) {
				return e.lock() == entity;
			});
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

			auto foundInSystem = std::find_if(systemEntities.begin(), systemEntities.end(),
				[&entity](const WeakPtr<Entity>& e) {
					return e.lock() == entity;
				});

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
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);
