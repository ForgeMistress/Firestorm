////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IInspectableObject.cpp
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IInspectableObject.h"

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

template <class Interface_t>
inline Interface_t* IInspectableObject::Inspect()
{
	return static_cast<Interface_t*>(DoInspect(Interface_t::MyType()));
}

template <class Interface_t>
inline const Interface_t* IInspectableObject::Inspect() const
{
	return reinterpret_cast<const Interface_t*>(DoInspect(Interface_t::MyType()));
}

inline void* IInspectableObject::Inspect(Type type)
{
	return DoInspect(type);
}
CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);