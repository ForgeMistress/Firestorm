///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Component
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

struct ComponentIDType
{
public:
	ComponentIDType(const char* name)
		: _hash(std::hash<String>()(name))
		, _name(name)
	{}

	operator size_t() const { return _hash; }
private:
	size_t _hash{ 0 };
	const char* _name;
};

using ComponentID = const ComponentIDType&;

#define FIRE_COMPONENT(TYPE)                   \
	virtual ComponentID ID() const             \
	{                                          \
		static ComponentIDType _id {#TYPE};    \
		return _id;                            \
	}                                          \

class IComponent
{
public:
	FIRE_COMPONENT(IComponent);

	using Instance = size_t;

protected:
	/**
		Make a new instance of the component.
	**/
	virtual Instance MakeNew() = 0;

public:
	/**
		Enumeration that defines different ways the destruction of entities can be handled by a component.
		A component should only lock itself into one type of destruction handler.
	 **/
	enum struct DestructionHandler
	{
		//< Used to signify that the component requires immediate attention when Entities are destroyed.
		kImmediate,

		//< A simple garbage collector is fine for this component.
		kGC
	};

	IComponent(DestructionHandler handler)
		: _dest(handler) {}
	virtual ~IComponent(){}

	/**
		Look up a component instance using the provided Entity. If one does not exist, then the implementing
		class needs to either make a new instance and return that or throw some kind of crashing error.
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

	/**
		This routine does exactly what it says on the tin. It clears out *everything*.
	 **/
	virtual void Clear() = 0;

	/**
		Retrieve thhe way this component should handle it when entities are destroyed.
	 **/
	DestructionHandler GetDestructionHandler() const { return _dest; }
private:
	DestructionHandler _dest;
};

/**
	\brief A component definition with the first SOA index being an Entity.

	This is pretty much the default component definition that you'll be getting the most
	use out of. The first template parameter specifies the mapping key (likely an Entity ;) )
	and the rest are the members. Uses the SOA container internally and exposes it
	with the member \c _this to your superclasses.
 **/
template<class... Members>
class Component : public IComponent
{
public:
	using Base = Component<Members...>;

	Component(EntityMgr& entityMgr, DestructionHandler handler)
	: IComponent(handler)
	, _eMgr(entityMgr)
	{
		if(GetDestructionHandler() == DestructionHandler::kImmediate)
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

	virtual ~Component()
	{
		if(GetDestructionHandler() == DestructionHandler::kImmediate)
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
		if(Contains(entity))
		{
			return _map[entity];
		}
		Instance i = MakeNew();

		FIRE_VALIDATE_COMPONENT(i);

		_this[0_soa][i] = entity;
		_map[entity] = i;
		return i;
	}

	virtual void Clear()
	{
		_this.Clear();
		_map.clear();
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
