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
#include "Component.h"
#include "Engine.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_MIRROR_DEFINE(Firestorm::Entity)
{
	_class.property("name",       &Entity::_name);
	_class.property("components", &Entity::_components);
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

bool Entity::AddComponent(Mirror::Type type)
{
	FIRE_BROKEN;
	FIRE_ASSERT(type.is_valid());
	Mirror::Instance componentInstance = type.create();
	if(componentInstance.is_valid())
	{
		return AddComponent(componentInstance.try_convert<Component>());
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
	return nullptr;
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

bool Entity::AddComponent(const RefPtr<Component>& component)
{
	for (auto com : _components)
	{
		if (com == component)
			return false;

		if (com->GetType() == component->GetType())
			return false;
	}
	_modified = true;

	if (_owningEngine)
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

CLOSE_NAMESPACE(Firestorm);