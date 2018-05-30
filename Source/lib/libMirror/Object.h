////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Object.h
//
//  The base object.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MIRROR_OBJECT_H_
#define MIRROR_OBJECT_H_
#pragma once

#include "rttr/registration"
#include "rttr/type"

OPEN_NAMESPACE(Elf);

class Object
{
	RTTR_ENABLE();

protected:
    Object(){}
    virtual ~Object(){}
};

class IInspectableObject
{
public:
	RTTR_ENABLE();

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

    virtual void* DoInspect(rttr::type rttrType)
    {
        return nullptr;
    }

protected:
    IInspectableObject(){}
    virtual ~IInspectableObject(){}
};

CLOSE_NAMESPACE(Elf);
#endif