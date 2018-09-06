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

/**
	Macro that provides more functions to a reflected object.

	static MyType   - Retrieves a consistent type.
	static Is       - Checks the type against all common forms.
	virtual GetType - Retrieve the type at the instance leve.
**/
#define ELF_MIRROR_DECLARE(ObjectType, ...)                                                                    \
	public:                                                                                                    \
		typedef ObjectType WhatIAm_t;                                                                          \
		class Registrar : public ::Elf::Mirror::RegistrarBase<ObjectType>									   \
		{																									   \
		public:																								   \
			Registrar();																					   \
		};																									   \
	public:                                                                                                    \
		static rttr::type  MyType()                     { return rttr::type::get<WhatIAm_t>(); }               \
		static  bool       Is(rttr::type rttrType)      { return WhatIAm_t::MyType() == rttrType; }            \
		virtual RTTR_INLINE rttr::type GetType() const  { return WhatIAm_t::MyType(); }                        \
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
		friend struct rttr::detail::constructor_invoker;

/**
	Define a class for the reflection system.
 **/
#define ELF_MIRROR_DEFINE(ObjectType) ELF_MIRROR_DEFINE_NAMED(ObjectType, #ObjectType)

/**
	Define the specifics of the class declaration while keying it to a specific name.
 **/
#define ELF_MIRROR_DEFINE_NAMED(ObjectType, ObjectName)			                   \
	ObjectType::Registrar::Registrar()											   \
	: RegistrarBase<ObjectType>(rttr::registration::class_<ObjectType>(ObjectName))

/**
	Register the class type to the reflection system. You will see this called in the lib*::Initialize functions.
 **/
#define ELF_MIRROR_REGISTER(ObjectType) ObjectType::Registrar()

/**
	Define the class level metadata while in an ELF_MIRROR_DEFINE* block.
 **/
#define ELF_CLASS_METADATA(...) ( __VA_ARGS__ )

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

template <class Class_Type>
class RegistrarBase
{
public:
	explicit RegistrarBase(rttr::registration::class_<Class_Type>& klass)
	: _class(klass)
	{
	}

	template<typename... Args>
	rttr::registration::class_<Class_Type>& operator()(Args&&...args) { return _class(args...); }

	template<typename... Args>
	rttr::registration::class_<Class_Type>& ClassInfo(Args&&...args) { return _class(args...); }

	template<
		typename... Args,
		typename acc_level = rttr::detail::public_access,
		typename Tp = typename std::enable_if<
			rttr::detail::contains<
				acc_level,
				rttr::detail::access_levels_list
			>::value
		>::type
	> rttr::registration::bind<rttr::detail::ctor, Class_Type, acc_level, Args...>
		Constructor(acc_level level = acc_level())
	{
		return _class.constructor<Args...,acc_level,Tp>(level);
	}

	template<
		typename F,
		typename acc_level = rttr::detail::public_access,
		typename Tp = typename std::enable_if<
			!rttr::detail::contains<
				F,
				rttr::detail::access_levels_list
			>::value
		>::type
	> rttr::registration::bind<rttr::detail::ctor_func, Class_Type, F, acc_level>
		Constructor(F func, acc_level level = acc_level())
	{
		return _class.constructor<F,acc_level,Tp>(func, level);
	}

	template<
		typename A,
		typename acc_level = rttr::detail::public_access,
		typename Tp = typename std::enable_if<
			rttr::detail::contains<
				acc_level,
				rttr::detail::access_levels_list
			>::value
		>::type
	> rttr::registration::bind<rttr::detail::prop, Class_Type, A, acc_level>
		Property(rttr::string_view name, A acc, acc_level level = acc_level())
	{
		return _class.property<A,acc_level,Tp>(name, acc, level);
	}

	template<
		typename A,
		typename acc_level = rttr::detail::public_access,
		typename Tp = typename std::enable_if<
			rttr::detail::contains<
				acc_level,
				rttr::detail::access_levels_list
			>::value
		>::type
	> rttr::registration::bind<rttr::detail::prop_readonly, Class_Type, A, acc_level>
		ReadOnlyProperty(rttr::string_view name, A acc, acc_level level = acc_level())
	{
		return _class.property_readonly<A,acc_level,Tp>(name, acc, level);
	}

	template<
		typename A1,
		typename A2,
		typename Tp = typename std::enable_if<
			!rttr::detail::contains<
				A2,
				rttr::detail::access_levels_list
			>::value
		>::type,
		typename acc_level = rttr::detail::public_access
	>
	rttr::registration::bind<rttr::detail::prop, Class_Type, A1, A2, acc_level> Property(rttr::string_view name, A1 getter, A2 setter, acc_level level = acc_level())
	{
		return _class.property<A1,A2,Tp,acc_level>(name, getter, setter, level);
	}

	template<
		typename F,
		typename acc_level = rttr::detail::public_access
	>
	rttr::registration::bind<rttr::detail::meth, Class_Type, F, acc_level> Method(rttr::string_view name, F f, acc_level level = acc_level())
	{
		return _class.method<F,acc_level>(name, f, level);
	}

	template<typename Enum_Type>
	rttr::registration::bind<rttr::detail::enum_, Class_Type, Enum_Type> Enumeration(rttr::string_view name)
	{
		return _class.enumeration<Enum_Type>(name);
	}

protected:
	rttr::registration::class_<Class_Type>& _class;
};

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
#define MIRROR_META_SAVE rttr::metadata(Elf::Mirror::PropMeta::SAVE, true)

/**
	 \macro MIRROR_META_SAVE

	 Tells the serializer that this property should be used to load data from the serialized object's file
	 to the object instance we're constructing.

	 Convenience wrapper around rttr::metadata(PropMeta::LOAD, true)
 **/
#define MIRROR_META_LOAD rttr::metadata(Elf::Mirror::PropMeta::LOAD, true)

/**
	 \macro MIRROR_META_SAVELOAD

	 Combination of \ref MIRROR_META_SAVE and \ref MIRROR_META_LOAD
 **/
#define MIRROR_META_SAVELOAD \
	rttr::metadata(Elf::Mirror::PropMeta::SAVE, true), \
	rttr::metadata(Elf::Mirror::PropMeta::LOAD, true)

/**
	 \macro MIRROR_META_DEPRECATED

	 Mark a property as deprecated.
	 Wrapper around rttr::metadata(PropMeta::kDeprecated, true)
 **/
#define MIRROR_META_DEPRECATED rttr::metadata(Elf::Mirror::PropMeta::DEPRECATED, true)

/**
	\macro MIRROR_META_REMAP

	Remap this class from another class. Useful if classes need to be renamed.

	Convenience wrapper around rttr::metadata(PropMeta::CLASS_REMAP, NEW_CLASSNAME)
	
	\code{.cpp}

	\endcode
 **/
#define MIRROR_META_REMAP( NEW_CLASSNAME ) rttr::metadata(Elf::Mirror::PropMeta::CLASS_REMAP, NEW_CLASSNAME)

/**
	\class Object
	
	The base object type that everything should derive from.
 **/
class Object
{
	ELF_MIRROR_DECLARE(Elf::Mirror::Object);
public:
	virtual ~Object();
};

/**
	\macro DOINSPECT_SIMPLE

	\code{.cpp}
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
		return static_cast< WhatIAm_t* >(this); \
	}

/**
	\macro DOINSPECT_INTERFACE

	\code{.cpp}
	class ISomeInterface
	{
		MIRROR_DECLARE(ISomeInterface);
	public:
		virtual void SomeMethod() = 0;
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
	private:
		virtual void SomeMethod()
		{
			// do stuff
		}
	};
	\endcode
 **/
#define DOINSPECT_INTERFACE(INTERFACE, ARG) \
	if(INTERFACE::MyType() == ARG) \
	{ \
		return static_cast< INTERFACE* >(this); \
	}

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif