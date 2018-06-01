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

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

/**
 *
 *  Object
 *
 *  The base object type that everything should derive from.
 *
 **/
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
 *
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
        return reinterpret_cast<Interface_t*>(DoInspect(Interface_t::get_type()));
    }

protected:
	virtual void* DoInspect(Type type);
	
    IInspectableObject(){}
    virtual ~IInspectableObject(){}
};

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif
