///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MappedComponentDefinition
//
//  Creates a definition of a component type that Entities can be tied to.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Universe 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_COMPONENTDEFINITION_H_
#define LIBEXISTENCE_COMPONENTDEFINITION_H_
#pragma once

#include <libCore/SOA.h>

#include "Entity.h"

OPEN_NAMESPACE(Firestorm);

#define FIRE_INVALID_COMPONENT FIRE_MAX_SOA_CAPACITY - 1

#define FIRE_VALIDATE_COMPONENT(i) \
	FIRE_ASSERT_MSG(i != FIRE_INVALID_COMPONENT, "the component is invalid "\
		"(side note... what the *actual* *hell* are you doing where you made *that* *many* components?!)")

class IComponentDefinition
{
public:
	using Instance = size_t;

protected:
	/**
		Make a new instance of the component.
	**/
	virtual Instance MakeNew() = 0;

public:
	/**
		Look up a component instance using the provided Entity. If one does not exist, then the implementing
		class needs to either make a new instance and return that or throow some kind of crashing error.
	**/
	virtual Instance Lookup(Entity entity) = 0;

	/**
		Query whether or not the provided Entity contains an instance of this component.
	**/
	virtual bool Contains(Entity entity) = 0;

	/**
		Assign an entity to have an instance of the component.
	
		\note If your component definition is a mapped type of definition, then it makes sense to return
		the mapped instance and throw out a warning of some kind for when you're being a bad programmer.

		\warning Will assert if thhere is no more room for components. However, the maximum number of component instances
		is pretty absurdly high (max of size_t) and I doubt that you'll ever have this problem.
	 **/
	virtual Instance Assign(Entity entity) = 0;
};

/**
	\brief A component definition with the first SOA index being an Entity.

	This is pretty much the default component definition that you'll be getting the most
	use out of.
 **/
template<class... Members>
class MappedComponentDefinition : public IComponentDefinition
{
public:
	using DefType = MappedComponentDefinition<Members...>;

	/**
		Used internally to signify that the definition is garbage collected.
	 **/
	static constexpr bool GarbageCollected = false;

	MappedComponentDefinition(EntityMgr& entityMgr, bool registerDestructor = false)
	: _eMgr(entityMgr)
	{
		if(registerDestructor)
		{
			_eMgr.RegisterDestructionCallback(this, [this](Entity entity) {
				// lookup the index of the provided entity
				size_t e = Lookup(entity);

				// nuke the reference to the old entity.
				_map.erase(entity);

				// remap so that the last entity equals the index of the old entity.
				_map[_this[0_soa][_this.Last()]] = e;

				// erase the old index. automatically decrements the size.
				_this.Erase(e);
			});
		}
	}

	virtual ~MappedComponentDefinition()
	{
		_eMgr.UnregisterDestructionCallback(this);
	}

	virtual Instance Lookup(Entity entity) final
	{
		auto found = _map.find(entity);
		if(found == _map.end())
		{
			return FIRE_INVALID_COMPONENT;
		}
		return found->second;
	}

	virtual bool Contains(Entity entity) final
	{
		return _map.find(entity) != _map.end();
	}

	virtual Instance Assign(Entity entity) final
	{
		if(_map.find(entity) != _map.end())
		{
			return _map[entity];
		}
		Instance i = MakeNew();

		FIRE_VALIDATE_COMPONENT(i);

		_this[0_soa][i] = entity;
		_map[entity] = i;
		return i;
	}

private:
	virtual Instance MakeNew() final
	{
		return MakeNew(_this.PushBack());
	}

	Instance MakeNew(size_t i)
	{
		return i;
	}

	EntityMgr& _eMgr;

	UnorderedMap<Entity, Instance> _map;
protected:
	SOA<Entity, Members...> _this;
};

CLOSE_NAMESPACE(Firestorm);
#endif
