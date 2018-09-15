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

#include <rttr/type.h>
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
#include <rttr/detail/base/core_prerequisites.h>
#include <rttr/policy.h>
#include <rttr/access_levels.h>
#include <rttr/detail/registration/bind_types.h>
#include <rttr/detail/registration/registration_executer.h>
#include <rttr/detail/default_arguments/default_arguments.h>
#include <rttr/detail/parameter_info/parameter_names.h>
#include <rttr/variant.h>

#include "MirrorMacros.h"

// Borrowed from https://stackoverflow.com/questions/401621/best-way-to-for-c-types-to-self-register-in-a-list
template<typename D>
struct AutomaticRegister
{
public:
	struct ExecRegister
	{
		ExecRegister()
		{
			D::DoRegisterReflection();
		}
	};
private:
	// will force instantiation of definition of static member
	template<ExecRegister&> struct ref_it {};

	static ExecRegister s_registerObject;
	static ref_it<s_registerObject> s_referrer;
};

OPEN_NAMESPACE(Firestorm);
OPEN_NAMESPACE(Mirror);

/**
	Typedef for rttr::type. Use this instead of rttr::type.
 **/
typedef rttr::type Type;

/**
	Typedef for rttr::instance. Use this instead of rttr::instance.
 **/
typedef rttr::instance Instance;

/**
	Typedef for rttr::variant. Use this instead of rttr::variant.
 **/
typedef rttr::variant Variant;

typedef rttr::property Property;

/**
	Typedef for rttr::constructor. Use instead of rttr::constructor.
 **/
typedef rttr::constructor Constructor;

/**
	Typedef for rttr::method. Use instead of rttr::method.
**/
typedef rttr::method Method;

/**
	Typedef for rttr::registration. Use this instead of rttr::registration.
 **/
typedef rttr::registration Registration;

/**
	Typedef for rttr::detail::metadata. Use this instead of rttr::detail::metadata.
 **/
typedef rttr::detail::metadata Metadata;

/**
	Alias for rttr::metadata function. Use this instead of rttr::metadata.
 **/
extern Metadata Meta(Variant key, Variant value);

/**
	\struct PropMeta

	Holds the properties metadata enumeration.
 **/
struct PropMeta
{
	/**
		\enum Type

		Enumeration that 
	 **/
	enum Type : uint8_t
	{
		SAVE        = 0, //< Mark a property as needing to be loaded from data files.
		LOAD        = 1, //< Mark a property as needing to be saved to data files.
		DEPRECATED  = 2, //< Mark a property as deprecated.
		CLASS_REMAP = 3  //< Use this to define another class name when renaming classes.
	};
};

/**
	\macro MIRROR_META_SAVE

	Tells the serializer that this property should be used to write data to the serialized object's file.

	Convenience wrapper around rttr::metadata(PropMeta::SAVE, true)
 **/
#define MIRROR_META_SAVE rttr::metadata(Firestorm::Mirror::PropMeta::SAVE, true)

/**
	 \macro MIRROR_META_SAVE

	 Tells the serializer that this property should be used to load data from the serialized object's file
	 to the object instance we're constructing.

	 Convenience wrapper around rttr::metadata(PropMeta::LOAD, true)
 **/
#define MIRROR_META_LOAD rttr::metadata(Firestorm::Mirror::PropMeta::LOAD, true)

/**
	 \macro MIRROR_META_SAVELOAD

	 Combination of \ref MIRROR_META_SAVE and \ref MIRROR_META_LOAD
 **/
#define MIRROR_META_SAVELOAD \
	rttr::metadata(Firestorm::Mirror::PropMeta::SAVE, true), \
	rttr::metadata(Firestorm::Mirror::PropMeta::LOAD, true)

/**
	 \macro MIRROR_META_DEPRECATED

	 Mark a property as deprecated.
	 Wrapper around rttr::metadata(PropMeta::kDeprecated, true)
 **/
#define MIRROR_META_DEPRECATED rttr::metadata(Firestorm::Mirror::PropMeta::DEPRECATED, true)

/**
	\macro MIRROR_META_REMAP

	Remap this class from another class. Useful if classes need to be renamed.

	Convenience wrapper around rttr::metadata(PropMeta::CLASS_REMAP, NEW_CLASSNAME)
	
	\code{.cpp}

	\endcode
 **/
#define MIRROR_META_REMAP( NEW_CLASSNAME ) rttr::metadata(Firestorm::Mirror::PropMeta::CLASS_REMAP, NEW_CLASSNAME)

/**
	\class Object
	
	The base object type that everything should derive from.
 **/
class Object
{
	FIRE_MIRROR_DECLARE(Firestorm::Mirror::Object);
public:
	virtual ~Object();
};

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Firestorm);


#endif