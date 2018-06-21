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

OPEN_NAMESPACE(Elf);

class Engine;
class Entity;

class System : public Mirror::Object,
               public Mirror::IInspectableObject
{
	MIRROR_DECLARE(System, Mirror::Object, Mirror::IInspectableObject);
public:
	System();
	virtual ~System();

	inline void AddToEngine(SharedPtr<Engine>& engine);
	inline bool Filter(SharedPtr<Entity>& entity) const;

protected:
	friend class Engine;

	virtual void OnAddToEngine() = 0;
	virtual void OnUpdate(float deltaT) = 0;
	virtual bool OnEntityFilter(SharedPtr<Entity>& entity) const = 0;

	void DoUpdate(float deltaT);
	bool AddEntity(SharedPtr<Entity>& entity);

	virtual void* DoInspect(Mirror::Type type);

private:
	WeakPtr<Engine> m_engine;
	Vector< WeakPtr<Entity> > m_entities;
};

CLOSE_NAMESPACE(Elf);
#endif