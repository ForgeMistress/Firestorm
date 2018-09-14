///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  System.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_SYSTEM_H_
#define LIBEXISTENCE_SYSTEM_H_
#pragma once

#include <libMirror/IInspectableObject.h>
#include <libMirror/EventDispatcher.h>

#include "Entity.h"

OPEN_NAMESPACE(Firestorm);

class Engine;

class SystemEvent
{
	FIRE_MIRROR_DECLARE(SystemEvent);
public:
	enum Type : char
	{
		kEvent_Modified,
		kEvent_EntityAdded,
		kEvent_EntityRemoved,
		kEvent_BeforeAddedToEngine,
		kEvent_OnAddedToEngine,
		kEvent_OnRemovedFromEngine
	};
	SystemEvent(Type type, Engine* engine);
	SystemEvent(Type type, Entity* entity);

	Type type;
	Engine* engine;
	Entity* entity;
};

class System : public Mirror::Object,
               public Mirror::IInspectableObject
{
	FIRE_MIRROR_DECLARE(System, Mirror::Object, Mirror::IInspectableObject);
public:
	System();
	virtual ~System();

	/**
		\function Filter

		Run the filtering routine to determine whether or not the \ref Entity will be able
		to be added to this system.
	 **/
	bool Filter(const Entity* entity) const;

	/**
		\function GetEngine

		Retrieve a \ref WeakPtr to the \ref Engine that holds this system.
	 **/
	const Engine* GetEngine() const;

	/**
		\function GetName

		Retrieve the name of this system.
	 **/
	const String& GetName() const;

	/**
		Set the name of this system.
	 **/
	void SetName(const String& name);

	/**
		Check whether or not this system contains the entity.
	 **/
	bool Contains(const Entity* entity) const;

	/**
		Pause the execution of this system temporarily.
	 **/
	void Pause();

protected:
	friend class Engine;

	/**
		Called when the \ref System is modified in any conceivable way. This function is called as part of the update loop and so can
		be reliably used to detect when objects are added or removed.
	 **/
	virtual void OnModified(double deltaT) {}

	/**
		Called when, for whatever reason, an entity is added to the internal entity list.
	 **/
	virtual void OnEntityAdded(const Entity* /*entity*/) {}

	/**
		Called when, for whatever reason, an entity is no longer a part of this system.

		\arg \c Entity The entity that has been removed from the system. Guaranteed to be valid so no validation is required.
	**/
	virtual void OnEntityRemoved(const Entity* /*entity*/) {}

	/**
		\function OnBeforeAddToEngine

		Callback function called before this \ref System is added to an \ref Engine. \ref GetEngine is guaranteed
		to be invalid at this point in time.
	 **/
	virtual void OnBeforeAddToEngine() {}

	/**
		Callback function called when this system is added to an \ref Engine. \ref GetEngine() is guaranteed to return a pointer
		to a valid \ref Engine instance that is, indeed, the \ref Engine instance that will hold this \ref System for the duration
		of its operation.
	 **/
	virtual void OnAddToEngine() {}

	/**
		Callback called when this system is removed from an engine. #GetEngine() is guaranteed to not be nullptr until
		after this function returns.
	 **/
	virtual void OnRemoveFromEngine() {}

	/**
		\function OnTick

		Callback function called by its holding Engine when it needs to be updated.

		\arg \c DeltaT The delta time that has passed between calls.
		\arg \c Entities A list of Entities that the System can act upon.
	 **/
	virtual void OnTick(double /*deltaT*/, const Vector<Entity*>& /*entities*/) {}

	/**
		\function OnEntityFilter

		This callback function is used to tell the \ref Engine whether or not the passed \ref Entity should become a 
		part of this system.

		\arg \c entity A WeakPtr to an Entity instance. Guaranteed to be valid and so no validity checking is required.

		\return shouldAdd true if this entity should be added and false if not.
	 **/
	virtual bool OnEntityFilter(const Entity* entity) const { return false; }

	virtual void* DoInspect(Mirror::Type type);

	EventDispatcher Dispatcher;

private:
	/* /!\ INTERNAL ONLY /!\ */
	void AddToEngine(Engine* engine);
	void Reset();

	// Reflected.
	String _name;

	// Runtime.
	bool            _modified;
	bool            _active;
	bool            _paused;
	Engine*         _engine;
	Vector<Entity*> _entities;
};

CLOSE_NAMESPACE(Firestorm);
#endif