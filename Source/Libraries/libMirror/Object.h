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

/**
	Macro that provides more functions to a reflected object.

	static MyType   - Retrieves a consistent type.
	static Is       - Checks the type against all common forms.
	virtual GetType - Retrieve the type at the instance leve.
**/
#define FIRE_MIRROR_DECLARE(ObjectType, ...)                                                                   \
	private:                                                                                                   \
		template <class T> friend struct Library;                                                              \
		using register_type = rttr::registration::class_<ObjectType>;                                          \
		static void DoRegisterReflection(register_type& Registrar);                                            \
	public:                                                                                                    \
		static void RegisterReflection();                                                                      \
		static rttr::type  MyType()                     { return rttr::type::get<ObjectType>(); }              \
		virtual RTTR_INLINE rttr::type GetType() const  { return ObjectType::MyType(); }                       \
		virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); } \
		virtual RTTR_INLINE void* get_ptr()             { return reinterpret_cast<void*>(this); }              \
		virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() {                                    \
			return {                                                                                           \
				reinterpret_cast<void*>(this),                                                                 \
				rttr::detail::get_type_from_instance(this)                                                     \
			};                                                                                                 \
		}                                                                                                      \
		using base_class_list = rttr::detail::type_list<__VA_ARGS__>;                                          \
	private:                                                                                                   \
		template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> 				   \
		friend struct rttr::detail::constructor_invoker

/**
	Define a class for the reflection system.
 **/
#define FIRE_MIRROR_DEFINE(ObjectType) FIRE_MIRROR_DEFINE_NAMED(ObjectType, #ObjectType)

#if defined FIRE_DEBUG || FIRE_RELEASE
	#define FIRE_MIRROR_DEFINE_NAMED(ObjectType, ObjectName)			                       \
		void ObjectType::RegisterReflection()                                                  \
		{                                                                                      \
			static bool s_alreadyRegistered = false;                                           \
			FIRE_ASSERT(s_alreadyRegistered == false &&                                        \
				"object of type '"#ObjectType"' is already registered for reflection");          \
			s_alreadyRegistered = true;                                                        \
			std::cout<<"    :: Registering Class Type: "<<ObjectName<<std::endl<<std::flush;   \
			ObjectType::register_type registration(ObjectName);                                \
			ObjectType::DoRegisterReflection(registration);                                    \
		}                                                                                      \
		void ObjectType::DoRegisterReflection(register_type& Class)
#elif defined FIRE_FINAL
	#define FIRE_MIRROR_DEFINE_NAMED(ObjectType, ObjectName)                               \
		void ObjectType::RegisterReflection()                                              \
		{                                                                                  \
			rttr::registration::class_<ObjectType> registration(ObjectName);               \
			ObjectType::DoRegisterReflection(registration);                                \
		}                                                                                  \
		void ObjectType::DoRegisterReflection(register_type& Class)
#endif

/**
	Define the class level metadata while in an FIRE_MIRROR_DEFINE* block.
 **/
#define FIRE_CLASS_METADATA(...) ( __VA_ARGS__ )

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
static Metadata Meta(Variant key, Variant value);

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