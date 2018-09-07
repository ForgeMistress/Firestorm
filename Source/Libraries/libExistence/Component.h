///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Component.h
//
//  A component in an entity that contains only data at the high level.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_COMPONENT_H_
#define LIBEXISTENCE_COMPONENT_H_
#pragma once

#include <libCore/RefPtr.h>
#include <libMirror/IInspectableObject.h>

OPEN_NAMESPACE(Firestorm);

class Entity;

/**
	\class Component

	A component is an object that contains data and is a part of an \ref Entity. \ref Components should not
	contain application logic within their class definitions.
 **/
class Component : public Mirror::Object,
                  public Mirror::IInspectableObject
{
	FIRE_MIRROR_DECLARE(Component, Mirror::Object, Mirror::IInspectableObject);
public:
	Component();
	virtual ~Component();

	inline const String& GetName() const;

	inline void SetName(const String& name);

	/**
		Retrieve the entity that this Component is a part of.

		\return entity The internally held WeakPtr<Entity> with const qualifiers.
	 **/
	inline const WeakPtr<Entity>& GetEntity() const;

	/**
		Retrieve the entity that this Component is a part of.

		\return entity The internally held WeakPtr<Entity> without const qualifiers.
	**/
	inline WeakPtr<Entity>& GetEntity();

protected:
	virtual void* DoInspect(Mirror::Type type);

private:
	String m_name;
	WeakPtr<Entity> m_entity;
};

CLOSE_NAMESPACE(Firestorm);
#endif