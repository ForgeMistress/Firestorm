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

OPEN_NAMESPACE(Elf);

class Object
{
protected:
    Object(){}
    virtual ~Object(){}
};

class InspectableObject : public Object
{
public:
    template <class Interface_t>
    void* Inspect()
    {
        return static_cast<Interface_t>(DoInspect(Interface_t::get_type()));
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
    InspectableObject(){}
    virtual ~InspectableObject(){}
};

CLOSE_NAMESPACE(Elf);
#endif

