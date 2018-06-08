////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Object.h
//
//  The base object.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_OBJECT_H_
#define LIBMIRROR_OBJECT_H_
#pragma once

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

/**
	Typedef for rttr::type. Use this instead of rttr::type.
 **/
typedef rttr::type Type;

/**
	Typedef for rttr::registration. Use this instead of rttr::registration.
 **/
typedef rttr::registration Registration;

#define MIRROR_DECLARE(objType, ...) \
	public: \
		static rttr::type MyType() { return rttr::type::get<objType>(); } \
		virtual rttr::type GetType() const { return rttr::type::get<objType>(); } \
		virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); }  \
		virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(this); } \
		virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() { return {reinterpret_cast<void*>(this), rttr::detail::get_type_from_instance(this)}; } \
		using base_class_list = rttr::detail::type_list<__VA_ARGS__>

struct TypeUtils
{
	/**
		Returns the rttr::type of an object without any qualifiers (const, pointer, etc...).
	**/
	static rttr::type get(void* obj);

	/**
		Returns the rttr::type of a template object without any qualifiers (const, pointer, etc...).
	 **/
	template<class T> static rttr::type get();
};

rttr::type TypeUtils::get(void* obj)
{
	return rttr::type::get(obj).get_raw_type();
}

template <class T>
rttr::type TypeUtils::get()
{
	return rttr::type::get<T>().get_raw_type();
}

/**
 *
 *  Object
 *
 *  The base object type that everything should derive from.
 *
 **/
class Object
{
	MIRROR_DECLARE(Object);
public:
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
	MIRROR_DECLARE(IInspectableObject);
public:
	/**
		Inspect an object to see if it implements the type defined in the template.
	 **/
    template <class Interface_t>
    Interface_t* Inspect()
    {
        return static_cast<Interface_t*>(DoInspect(Interface_t::MyType()));
    }

	/**
		Inspect an object to see if it implements the type defined in the template.
	 **/
    template <class Interface_t>
    const Interface_t* Inspect() const
    {
        return reinterpret_cast<Interface_t*>(DoInspect(Interface_t::MyType()));
    }

	/**
		Inspect an object to see if it implements the provided type.
	 **/
	void* Inspect(Type type)
	{
		return DoInspect(type);
	}

protected:
	/** Override this in implementing classes to provide introspection to the type. **/
	virtual void* DoInspect(Type type);
	
    IInspectableObject(){}
    virtual ~IInspectableObject(){}
};

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif
