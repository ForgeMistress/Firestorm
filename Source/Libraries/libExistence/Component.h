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
#include <libCore/IRefCounted.h>
#include <libCore/WeakPtr.h>

OPEN_NAMESPACE(Firestorm);

class Entity;

struct ComponentMetadata
{
	enum List
	{
		kPlural     //< Metadata that allows more than one component of this type to exist in an Entity.
	};
};

/**
	\class Component

	A component is an object that contains data and is a part of an \ref Entity. \ref Components should not
	contain application logic within their class definitions.
 **/
/*class IComponent
{
	FIRE_MIRROR_DECLARE(IComponent);
public:
	IComponent();
	virtual ~IComponent();
};*/

CLOSE_NAMESPACE(Firestorm);
#endif
