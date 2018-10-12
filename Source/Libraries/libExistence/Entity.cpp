///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Entity.cpp
//
//  A collection of components.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Entity.h"
#include "Universe.h"
#include "ComponentDefinition.h"

#include <libCore/Logger.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityMgr::EntityMgr(UUIDMgr& uuidMgr)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entity EntityMgr::SpawnEntity(EntityData* data) const
{
	Entity out ={ 0,0 };
	/*if(!_deadEntities.empty())
	{
		out = _deadEntities.back();
		_deadEntities.pop_back();
	}
	else
	{
		out = _uuidMgr.Get();
	}

	if(data)
	{
		BuildEntity(out, data);
	}*/
	return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::DespawnEntity(Entity entity)
{
	_deadEntities.push_back(entity);
	DispatchDestruction(entity);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::RegisterDestructionCallback(void* registrant, DestructionCallback callback)
{
	_destructionCallbacks.push_back({
		callback,
		registrant
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::UnregisterDestructionCallback(void* registrant)
{
	for(size_t i=0; i<_destructionCallbacks.size(); ++i)
	{
		if(_destructionCallbacks[i].Registrant == registrant)
		{
			_destructionCallbacks.erase(_destructionCallbacks.begin() + i);
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EntityMgr::IsAlive(Entity entity)
{
	for(size_t i=0; i<_deadEntities.size(); ++i)
	{
		if(_deadEntities[i].id == entity.id)
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::DispatchDestruction(Entity entity)
{
	for(size_t i=0; i<_destructionCallbacks.size(); ++i)
	{
		_destructionCallbacks[i].Callback(entity);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::BuildEntity(Entity entity, EntityData* data) const
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*const Entity Entity::dummy(0, 0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entity EntityMgr::Create()
{
	ID idx;
	if(_freeIndices.size() > FIRE_MIN_FREE_INDICES)
	{
		idx = _freeIndices.front();
		_freeIndices.pop_front();
	}
	else
	{
		_generations.push_back(0);
		idx = _generations.size() - 1;
		FIRE_ASSERT(idx < (1 << ENT_INDEX_BITS));
	}

	return Entity(idx, _generations[idx]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool EntityMgr::IsAlive(const Entity& entity)
{
	return _generations[entity.Index()] == entity.Generation();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EntityMgr::Destroy(const Entity& entity)
{
	const uint32_t idx = entity.Index();
	++_generations[idx];
	_freeIndices.push_back(idx);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*FIRE_MIRROR_DEFINE(Firestorm::Entity)
{
	Class.property("name",       &Entity::_name);
	Class.property("components", &Entity::_components);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entity::Entity()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entity::~Entity()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity::AddComponent(const Mirror::Type& type)
{
	FIRE_ASSERT(type.is_valid());
	FIRE_ASSERT(type.is_derived_from<Component>());

	auto constructors = type.get_constructors();

	if(HasComponentOfType(type))
	{
		// check if it's plural
		Mirror::Variant pluralMeta = type.get_metadata(ComponentMetadata::kPlural);
		bool isPlural = pluralMeta.is_valid() && pluralMeta.get_value<bool>() == true;
		if(!isPlural)
		{
			return false;
		}
	}

	Mirror::Variant componentVar = type.create({});
	if(componentVar.is_valid())
	{
		return AddComponent(RefPtr<Component>(componentVar.convert<Component*>()));
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity::RemoveComponent(const String& componentName)
{

	auto findFunction = [&componentName](const RefPtr<Component>& c) {
		return c->GetName() == componentName;
	};
	auto found = std::find_if(_components.begin(), _components.end(), findFunction);
	if(found != _components.end())
	{
		_components.erase(found);

		if(_owningEngine)
			_owningEngine->ModifyEntity(this);

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity::RemoveComponent(Mirror::Type ofType)
{
	auto findFunction = [&ofType](const RefPtr<Component>& c) {
		return c->GetType() == ofType;
	};
	auto found = std::find_if(_components.begin(), _components.end(), findFunction);
	if(found != _components.end())
	{
		_components.erase(found);

		if(_owningEngine)
			_owningEngine->ModifyEntity(this);

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t Entity::GetNumComponents() const
{
	return _components.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WeakPtr<Component> Entity::GetComponent(size_t index) const
{
	FIRE_ASSERT(index < _components.size());
	return _components[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WeakPtr<Component> Entity::GetComponent(const String& name) const
{
	auto findFunc = [&name](const RefPtr<Component>& component) {
		return component->GetName() == name;
	};
	auto found = std::find_if(_components.begin(), _components.end(), findFunc);
	if(found != _components.end())
	{
		return (*found);
	}
	return RefPtr<Component>(nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector<WeakPtr<Component>> Entity::GetComponents(Mirror::Type type) const
{
	Vector<WeakPtr<Component>> output;
	for(auto component : _components)
	{
		if(component->Inspect(type) != nullptr)
			output.push_back(component);
	}
	return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* Entity::DoInspect(Mirror::Type type)
{
	for(auto component : _components)
	{
		void* output = component->Inspect(type);
		if(output != nullptr)
		{
			return output;
		}
	}
	return IInspectableObject::DoInspect(type);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// INTERNAL ONLY! //
bool Entity::AddComponent(const RefPtr<Component>& component)
{
	FIRE_ASSERT(component);
	if (!component)
		return false;
	for(auto com : _components)
	{
		if(com == component)
			return false;
	}
	_modified = true;

	if(_owningEngine)
		_owningEngine->ModifyEntity(this);

	_components.push_back(component);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity::RemoveComponent(const RefPtr<Component>& component)
{
	auto found = std::find(_components.begin(), _components.end(), component);
	if(found != _components.end())
	{
		_modified = true;

		_components.erase(found);

		if(_owningEngine)
			_owningEngine->ModifyEntity(this);

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Entity::HasComponentOfType(Mirror::Type type) const
{
	for(auto component : _components)
	{
		Mirror::Type componentType = component->GetType();
		if(type == componentType)
		{
			return true;
		}
	}
	return false;
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);
