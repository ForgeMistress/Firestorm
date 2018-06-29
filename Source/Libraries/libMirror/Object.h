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

#define MIRROR_DEFINE_NAMED(OBJ_TYPE, NAME) \
	rttr::registration::class_<OBJ_TYPE>( NAME )

#define MIRROR_DEFINE(OBJ_TYPE) \
	MIRROR_DEFINE_NAMED(OBJ_TYPE, #OBJ_TYPE)

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

/**
	Macro that provides more functions to a reflected object.

	static MyType - Retrieves a consistent type.
	static Is     - Checks the type against all common forms.
	virtual GetType - Retrieve the type at the instance leve.
 **/
#define MIRROR_DECLARE(objType, ...) \
	private: \
		typedef objType WhatIAm_t; \
	public: \
		static rttr::type MyType() { return rttr::type::get<WhatIAm_t>(); } \
		static  bool       Is(rttr::type rttr_t)        { return WhatIAm_t::MyType() == rttr_t; } \
		virtual RTTR_INLINE rttr::type GetType() const  { return WhatIAm_t::MyType(); } \
		virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); }  \
		virtual RTTR_INLINE void* get_ptr()             { return reinterpret_cast<void*>(this); } \
		virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() { \
			return { \
				reinterpret_cast<void*>(this), \
				rttr::detail::get_type_from_instance(this) \
			}; \
		} \
		using base_class_list = rttr::detail::type_list<__VA_ARGS__>; \
	private:

/**
	\enum
 **/
struct PropMeta
{
	enum Type
	{
		SAVE,
		LOAD,
		DEPRECATED, //< Mark a property as deprecated.
		CLASS_REMAP  //< Use this to define another class name when renaming classes.
	};
};

/**
	\macro MIRROR_META_SAVE
	Tells the serializer that this property should be used to write data to the serialized object's file.

	Wrapper around rttr::metadata(PropMeta::kSave, true)
 **/
#define MIRROR_META_SAVE rttr::metadata(PropMeta::SAVE, true)

 /**
	 \macro MIRROR_META_SAVE
	 Tells the serializer that this property should be used to load data from the serialized object's file
	 to the object instance we're constructing.

	 Wrapper around rttr::metadata(PropMeta::kLoad, true)
 **/
#define MIRROR_META_LOAD rttr::metadata(PropMeta::LOAD, true)

 /**
	 \macro MIRROR_META_SAVELOAD
	 Combination of \ref MIRROR_META_SAVE and \ref MIRROR_META_LOAD
 **/
#define MIRROR_META_SAVELOAD \
	rttr::metadata(PropMeta::SAVE, true), \
	rttr::metadata(PropMeta::LOAD, true)

 /**
	 \macro MIRROR_META_DEPRECATED
	 Mark a property as deprecated.
	 Wrapper around rttr::metadata(PropMeta::kDeprecated, true)
 **/
#define MIRROR_META_DEPRECATED rttr::metadata(PropMeta::DEPRECATED, true)

/**
	\class Object
	
	The base object type that everything should derive from.
 **/
class Object
{
	MIRROR_DECLARE(Object);
public:
    Object(){}
    virtual ~Object(){}
};

/**
	\macro DOINSPECT_SIMPLE

	class SomeClass : public Mirror::Object,
                      public Mirror::IInspectableObject
	{
		MIRROR_DECLARE(SomeClass, Mirror::Object, Mirror::IInspectableObject, ISomeInterface);
	protected:
		virtual void* DoInspect(Mirror::Type type)
		{
			DOINSPECT_SIMPLE(type);
			return IInspectableObject::DoInspect(type);
		}
	};
	\endcode
 **/
#define DOINSPECT_SIMPLE(ARG) \
	if(WhatIAm_t::MyType() == ARG) \
	{ \
		return static_cast<WhatIAm_t*>(this); \
	}

/**
	\macro DOINSPECT_INTERFACE

	\code{.cpp}
	class ISomeInterface
	{
		MIRROR_DECLARE(ISomeInterface);
	};
	class SomeClass : public Mirror::Object,
                      public Mirror::IInspectableObject,
                      public ISomeInterface
	{
		MIRROR_DECLARE(SomeClass, Mirror::Object, Mirror::IInspectableObject, ISomeInterface);
	protected:
		virtual void* DoInspect(Mirror::Type type)
		{
			DOINSPECT_SIMPLE(type);
			DOINSPECT_INTERFACE(ISomeInterface, type);
			return IInspectableObject::DoInspect(type);
		}
	};
	\endcode
 **/
#define DOINSPECT_INTERFACE(INTERFACE, ARG) \
	if(INTERFACE::MyType() == ARG) \
	{ \
		return static_cast<INTERFACE*>(this); \
	}

/**
	\class IInspectableObject
	
	An abstract class for an object that can have its interfaces inspected. Objects that derive from
	this interface must override DoInspect and check the rttr::type that is passed in. If the object matches
	the type, then it must return the pointer to the object static_casted to the apropriate type.
 **/
class IInspectableObject : public Object
{
	MIRROR_DECLARE(IInspectableObject, Object);
public:
	/**
		Inspect an object to see if it implements the type defined in the template.
	 **/
    template <class Interface_t>
	inline Interface_t* Inspect();

	/**
		Inspect an object to see if it implements the type defined in the template.
	 **/
    template <class Interface_t>
	inline const Interface_t* Inspect() const;

	/**
		Inspect an object to see if it implements the provided type.
	 **/
	inline void* Inspect(Type type);

protected:
	/** Override this in implementing classes to provide introspection to the type. **/
	virtual void* DoInspect(Type type);
	
    IInspectableObject(){}
    virtual ~IInspectableObject(){}
};

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

#endif
