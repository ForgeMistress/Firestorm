////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Object.h
//
//  The base object.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_OBJECT_H_
#define LIBMIRROR_OBJECT_H_
#pragma once

#include <rttr/registration>
#include <rttr/type>

OPEN_NAMESPACE(Elf);

class Object
{
	RTTR_ENABLE();

protected:
    Object(){}
    virtual ~Object(){}
};

/**
 *
 *  IInspectableObject
 *
 *  An interface for an object that can have its interfaces inspected. Objects that derive from
 *  this interface must override DoInspect and check the rttr::type that is passed in. If the object matches
 *  the type, then it must return the pointer to the object static_casted to the apropriate type.
 **/
class IInspectableObject
{
	RTTR_ENABLE();
public:
    template <class Interface_t>
    Interface_t* Inspect()
    {
        return static_cast<Interface_t*>(DoInspect(Interface_t::get_type()));
    }

    template <class Interface_t>
    const Interface_t* Inspect() const
    {
        return const_cast<const Interface_t*>(DoInspect(Interface_t::get_type()));
    }

protected:
	virtual void* DoInspect(rttr::type rttrType)
	{
		if(rttrType == IInspectableObject::get_type())
		{
			return static_cast<IInspectableObject*>(this);
		}
		return nullptr;
	}

    IInspectableObject(){}
    virtual ~IInspectableObject(){}
};

CLOSE_NAMESPACE(Elf);
#endif