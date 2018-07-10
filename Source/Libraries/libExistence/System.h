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

OPEN_NAMESPACE(Elf);

class Engine;
class Entity;

class System : public Mirror::IInspectableObject
{
	MIRROR_DECLARE(System, Mirror::IInspectableObject);
public:
	System();
	virtual ~System();

	inline void AddToEngine(SharedPtr<Engine>& engine);
	inline bool Filter(const SharedPtr<Entity>& entity) const;

	inline const WeakPtr<Engine>& GetEngine() const;
	inline const String&          GetName() const;

	inline bool Contains(const SharedPtr<Entity>& entity) const;

protected:
	friend class Engine;

	/**
		\function OnEntityAdded

		Called when, for whatever reason, an entity is added to the internal entity list.
	 **/
	virtual void OnEntityAdded(const SharedPtr<Entity>& /*entity*/) {}

	/**
		\function OnAddToEngine

		Callback function called when this system is added to an Engine. GetEngine() is guaranteed to not be nullptr.
	 **/
	virtual void OnAddToEngine() {}

	/**
		\function OnUpdate

		Function called when the \ref System needs to be updated.
	 **/
	virtual void OnUpdate(float /*deltaT*/, const Vector< WeakPtr<Entity> >& /*entities*/) {}

	/**
		\function OnEntityRemoved

		Called when, for whatever reason, an entity is no longer a part of this system.
	 **/
	virtual void OnEntityRemoved(const WeakPtr<Entity>& /*entity*/) {}

	/**
		\function OnEntityFilter

		This callback function is used to tell the \ref Engine whether or not the passed \ref Entity should become a 
		part of this system.

		\return true if this entity should be added and false if not.
	 **/
	virtual bool OnEntityFilter(const SharedPtr<Entity>& entity) const { return false; }

	void DoUpdate(float deltaT);
	bool AddEntity(SharedPtr<Entity>& entity);

	virtual void* DoInspect(Mirror::Type type);

private:
	// Reflected.
	String m_name;
	bool m_modified;
	WeakPtr<Engine> m_engine;
	Vector<WeakPtr<Entity> > m_entities;
};

const WeakPtr<Engine>& System::GetEngine() const
{ 
	return m_engine;
}

const String& System::GetName() const 
{ 
	return m_name;
}

bool System::Contains(const SharedPtr<Entity>& entity) const 
{ 
	return std::find(m_entities.begin(), m_entities.end(), m_entities) != m_entities.end(); 
}

CLOSE_NAMESPACE(Elf);
#endif