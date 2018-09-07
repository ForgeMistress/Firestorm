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
	SystemEvent(Type type, WeakPtr<Engine> engine);
	SystemEvent(Type type, WeakPtr<Entity> entity);

	Type type;
	WeakPtr<Engine> engine;
	WeakPtr<Entity> entity;
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
	bool Filter(const WeakPtr<Entity>& entity) const;

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
		\function SetName

		Set the name of this system.
	**/
	void SetName(const String& name);

	/**
		\function Contains

		Check whether or not this system contains the entity.
	 **/
	bool Contains(const WeakPtr<Entity>& entity) const;

	/**
		\function Pause

		Pause the execution of this system temporarily.
	 **/
	void Pause();

protected:
	friend class Engine;

	/**
		\function OnModified

		Called when the \ref System is modified in any conceivable way. This function is called as part of the update loop and so can
		be reliably used to detect when objects are added or removed.
	 **/
	virtual void OnModified(double deltaT) {}

	/**
		\function OnEntityAdded

		Called when, for whatever reason, an entity is added to the internal entity list.
	 **/
	virtual void OnEntityAdded(const WeakPtr<Entity>& /*entity*/) {}

	/**
		\function OnEntityRemoved

		Called when, for whatever reason, an entity is no longer a part of this system.

		\arg \c Entity The entity that has been removed from the system. Guaranteed to be valid so no validation is required.
	**/
	virtual void OnEntityRemoved(const WeakPtr<Entity>& /*entity*/) {}

	/**
		\function OnBeforeAddToEngine

		Callback function called before this \ref System is added to an \ref Engine. \ref GetEngine is guaranteed
		to be invalid at this point in time.
	 **/
	virtual void OnBeforeAddToEngine() {}

	/**
		\function OnAddToEngine

		Callback function called when this system is added to an \ref Engine. \ref GetEngine() is guaranteed to return a pointer
		to a valid \ref Engine instance that is, indeed, the \ref Engine instance that will hold this \ref System for the duration
		of its operation.
	 **/
	virtual void OnAddToEngine() {}

	/**
		\function OnRemoveFromEngine

		Callback called when this system is removed from an engine. \ref GetEngine() is guaranteed to not be nullptr until
		after this function returns.
	 **/
	virtual void OnRemoveFromEngine() {}

	/**
		\function OnUpdate

		Callback function called by its holding \ref Engine when it needs to be updated.

		\arg \c DeltaT The delta time that has passed between calls.
		\arg \c Entities A list of \ref Entities that the \ref System can act upon.
	 **/
	virtual void OnUpdate(double /*deltaT*/, const Vector<WeakPtr<Entity>>& /*entities*/) {}

	/**
		\function OnEntityFilter

		This callback function is used to tell the \ref Engine whether or not the passed \ref Entity should become a 
		part of this system.

		\arg \c entity A \ref WeakPtr to an \ref Entity instance. Guaranteed to be valid and so no validity checking is required.

		\return true if this entity should be added and false if not.
	 **/
	virtual bool OnEntityFilter(const WeakPtr<Entity>& entity) const { return false; }

	virtual void* DoInspect(Mirror::Type type);

	EventDispatcher Dispatcher;

private:
	/* /!\ INTERNAL ONLY /!\ */
	void AddToEngine(Engine* engine);
	void Reset();
	bool AddEntity(WeakPtr<Entity>& entity);

	// Reflected.
	String _name;

	// Runtime.
	bool                    _modified;
	bool                    _active;
	bool                    _paused;
	Engine*                 _engine;
	Vector<WeakPtr<Entity>> _entities;
};

CLOSE_NAMESPACE(Firestorm);
#endif