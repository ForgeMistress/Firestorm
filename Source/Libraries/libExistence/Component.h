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

OPEN_NAMESPACE(Elf);

class Entity;

/**
	\class Component

	A component is an object that contains data and is a part of an Entity. Components should not
	contain application logic within their class definitions.
 **/
class Component : public Mirror::IInspectableObject
{
	MIRROR_DECLARE(Component, Mirror::IInspectableObject);
public:
	Component();
	virtual ~Component();

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
	WeakPtr<Entity> m_entity;
};

CLOSE_NAMESPACE(Elf);
#endif