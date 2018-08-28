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

#define ELF_MIRROR_DEFINE_NAMED(OBJ_TYPE, NAME) \
	rttr::registration::class_<OBJ_TYPE>( NAME )

#define ELF_MIRROR_DEFINE(OBJ_TYPE) \
	ELF_MIRROR_DEFINE_NAMED(OBJ_TYPE, #OBJ_TYPE)

#define ELF_CAT_IMPL(a,b) a##b
#define ELF_CAT(a,b) ELF_CAT_IMPL(a,b)

#define ELF_MIRROR_REGISTRATION \
static void elf_auto_register_reflection_function_();                               \
namespace                                                                           \
{                                                                                   \
    struct elf__auto__register__                                                    \
    {                                                                               \
        elf__auto__register__()                                                     \
        {                                                                           \
            elf_auto_register_reflection_function_();                               \
        }                                                                           \
    };                                                                              \
}                                                                                   \
static const elf__auto__register__ ELF_CAT(auto_register__,__LINE__);               \
static void elf_auto_register_reflection_function_()

#define ELF_MIRROR_DEFINE_(OBJ_TYPE) ELF_MIRROR_DEFINE_NAMED_(OBJ_TYPE, #OBJ_TYPE)
#define ELF_MIRROR_DEFINE_NAMED_(OBJ_TYPE, OBJ_NAME) \
OBJ_TYPE::ElfRegister OBJ_TYPE::StaticHolder::Registrar; \
OBJ_TYPE::ElfRegister::ElfRegister() \
{ \
	Register(rttr::registration::class_<OBJ_TYPE>(OBJ_NAME)); \
} \
void OBJ_TYPE::ElfRegister::Register(rttr::registration::class_<OBJ_TYPE>& CLASS)

#define ELF_MIRROR_DECLARE_(OBJ_TYPE, ...)                                                                      \
	private:                                                                                                   \
		typedef OBJ_TYPE WhatIAm_t;                                                                            \
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
		RTTR_REGISTRATION_FRIEND;                                                                              \
		struct ElfRegister																					   \
		{																									   \
			ElfRegister();																					   \
			void Register(rttr::registration::class_<OBJ_TYPE>& CLASS);                                        \
		};																									   \
		struct StaticHolder																					   \
		{																									   \
			static ElfRegister Registrar;																	   \
		};																									   \
	private:

/**
	Macro that provides more functions to a reflected object.

	static MyType   - Retrieves a consistent type.
	static Is       - Checks the type against all common forms.
	virtual GetType - Retrieve the type at the instance leve.
**/
#define ELF_MIRROR_DECLARE(OBJ_TYPE, ...)                                                                      \
	private:                                                                                                   \
		typedef OBJ_TYPE WhatIAm_t;                                                                            \
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
		RTTR_REGISTRATION_FRIEND;                                                                              \
	private:

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

/**
	Typedef for rttr::type. Use this instead of rttr::type.
 **/
typedef rttr::type Type;

/**
	Typedef for rttr::variant. Use this instead of rttr::variant.
 **/
typedef rttr::variant Variant;

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
static Metadata Property(Variant key, Variant value);

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
	//ELF_MIRROR_DECLARE(Elf::Mirror::Object);
private:                                                                                                   \
	typedef Object WhatIAm_t;                                                                            \
public:                                                                                                    \
	static rttr::type  MyType() { return rttr::type::get<WhatIAm_t>(); }               \
	static  bool       Is(rttr::type rttrType) { return WhatIAm_t::MyType() == rttrType; }            \
	virtual RTTR_INLINE rttr::type GetType() const { return WhatIAm_t::MyType(); }                        \
	virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); } \
	virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(this); }              \
	virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() {
	\
		return { \
		reinterpret_cast<void*>(this),                                                                 \
		rttr::detail::get_type_from_instance(this)                                                     \
	};                                                                                                 \
}                                                                                                      \
using base_class_list = rttr::detail::type_list<>;                                          \
RTTR_REGISTRATION_FRIEND;                                                                              \
private:
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