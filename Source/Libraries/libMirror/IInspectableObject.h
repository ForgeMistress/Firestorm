////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//<
//<  \file IInspectableObject.h
//<
//<  Contains \ref IInspectableObject.
//<
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_IINSPECTABLEOBJECT_H_
#define LIBMIRROR_IINSPECTABLEOBJECT_H_
#pragma once

#include "Object.h"

OPEN_NAMESPACE(Firestorm);
OPEN_NAMESPACE(Mirror);

/**
\class IInspectableObject

An abstract class for an object that can have its interfaces inspected. Objects that derive from
this interface must override DoInspect and check the rttr::type that is passed in. If the object matches
the type, then it must return the pointer to the object static_casted to the apropriate type.
**/
class IInspectableObject
{
	FIRE_MIRROR_DECLARE(IInspectableObject);
public:
	/**
	Inspect an object to see if it implements the type defined in the template.
	**/
	template <class Interface_t>
	Interface_t* Inspect();

	/**
		Inspect an object to see if it implements the type defined in the template.
	**/
	template <class Interface_t>
	const Interface_t* Inspect() const;

	/**
		Inspect an object to see if it implements the provided type.
	**/
	void* Inspect(Type type);

protected:
	/** Override this in implementing classes to provide introspection to the type. **/
	virtual void* DoInspect(Type type);

	IInspectableObject() {}
	virtual ~IInspectableObject() {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Interface_t>
Interface_t* IInspectableObject::Inspect()
{
	return static_cast<Interface_t*>(DoInspect(Interface_t::MyType()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class Interface_t>
const Interface_t* IInspectableObject::Inspect() const
{
	return reinterpret_cast<const Interface_t*>(DoInspect(Interface_t::MyType()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Firestorm);

#endif