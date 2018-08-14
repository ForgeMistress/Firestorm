///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TypeTraits.cpp
//
//  Contains the traits for types within static templated structs.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TypeAbbreviationTraits.h"
#include "IDocument.h"

OPEN_NAMESPACE(Elf);

template <class T>
const char* TypeTraits<T>::GetAbbreviation()
{
	static const char* str = "<unknown>";
	return str;
}

template <class T>
const char* TypeTraits<T>::GetTypeString()
{
	static const char* str = "<unknown>";
	return str;
}

template <class T>
const Mirror::Type TypeTraits<T>::GetMirrorType()
{
	return rttr::type::get<void>();
}

template <class T>
Result<T, Error> TypeTraits<T>::ReadFromDocument(IDocument* reader, const String& key)
{
	return ELF_ERROR(TraitsErrors::ERROR, "can not read unknown type.");
}

template <class T>
Result<void, Error> TypeTraits<T>::WriteToDocument(IDocument* reader, const String& key, T value)
{
	return ELF_ERROR(TraitsErrors::ERROR, "can not write unknown type.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Void specialization
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* TypeTraits<void>::GetAbbreviation()
{
	static const char* str = "void";
	return str;
}

const char* TypeTraits<void>::GetTypeString()
{
	static const char* str = "void";
	return str;
}

const Mirror::Type TypeTraits<void>::GetMirrorType()
{
	return Mirror::Type::get<void>();
}

Result<void, Error> TypeTraits<void>::ReadFromDocument(IDocument*, const String&, void*)
{
	return ELF_ERROR(TraitsErrors::READ_ERROR, "could not read 'void' type from document.");
}

Result<void, Error> TypeTraits<void>::WriteToDocument(IDocument*, const String&, void*)
{
	return ELF_ERROR(TraitsErrors::WRITE_ERROR, "could not write 'void' type to document.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  int8_t specialization
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TRAITS_IMPL(TEMPLATE_TYPE, ABBREVIATION_STRING, FUNCTION_POSTFIX) \
const char* TypeTraits<TEMPLATE_TYPE>::GetAbbreviation() { \
	static const char* str = ABBREVIATION_STRING; \
	return str; \
} \
const char* TypeTraits<TEMPLATE_TYPE>::GetTypeString() { \
	static const char* str = #TEMPLATE_TYPE; \
	return str; \
} \
const Mirror::Type TypeTraits<TEMPLATE_TYPE>::GetMirrorType() { return Mirror::Type::get<TEMPLATE_TYPE>(); } \
Result<void, Error> TypeTraits<TEMPLATE_TYPE>::ReadFromDocument(IDocument* reader, const String& key, TEMPLATE_TYPE& outValue) \
{ \
	assert(reader && "document reader is nullptr in Traits<"#TEMPLATE_TYPE">::ReadFromDocument."); \
	int8_t val = 0; \
	Result<TEMPLATE_TYPE, Error> res = reader->Read##FUNCTION_POSTFIX(key.c_str()); \
	if(res.has_value()) \
	{ \
		return Result<void,Error>(); \
	} \
	return ELF_ERROR(TraitsErrors::READ_ERROR, "could not read "#TEMPLATE_TYPE" from document with key '" + key + "'"); \
} \
Result<void, Error> TypeTraits<TEMPLATE_TYPE>::WriteToDocument(IDocument* writer, const String& key, TEMPLATE_TYPE value) \
{ \
	assert(writer && "document writer is nullptr in Traits<"#TEMPLATE_TYPE">::WriteToDocument."); \
	Result<IDocument*, Error> res = writer->Write##FUNCTION_POSTFIX(key.c_str(), value); \
	if(res.has_value() && res.value() == writer) \
	{ \
		return Result<void, Error>(); \
	} \
	return ELF_ERROR(TraitsErrors::WRITE_ERROR, "could not write '"#TEMPLATE_TYPE"' to document with key '" + key + "'"); \
}

TRAITS_IMPL(int8_t, "i8", Int8)
TRAITS_IMPL(uint8_t, "ui8", UInt8)
TRAITS_IMPL(int16_t, "i16", Int16)



const char* TypeTraits<Mirror::Object*>::GetAbbreviation()
{
	static const char* str = "obj";
	return str;
}
const char* TypeTraits<Mirror::Object*>::GetTypeString()
{
	static const char* str = "Object";
	return str;
}

const Mirror::Type TypeTraits<Mirror::Object*>::GetMirrorType()
{
	return rttr::type::get<Mirror::Object>();
}

Result<void, Error> TypeTraits<Mirror::Object*>::ReadFromDocument(IDocument* reader, const String& key, Mirror::Object* outValue)
{

	Mirror::Type objType = outValue->MyType();
	rttr::array_range<rttr::property> props = 
		objType.get_properties(rttr::filter_item::instance_item | 
                               rttr::filter_item::static_item |
			                   rttr::filter_item::public_access |
                               rttr::filter_item::non_public_access);
	reader->EnterSubsection(key.c_str());
	for(rttr::property prop : props)
	{
		rttr::variant meta = prop.get_metadata(Mirror::PropMeta::LOAD);
		if(meta.to_bool() == true)
		{
			rttr::string_view name = prop.get_name();
			Result<Mirror::Type, Error> propType = reader->GetType(name.to_string().c_str());
			if(propType.has_value())
			{

			}
			//rttr::variant prop
			//rttr::type propType = prop.get_type();
			//if(propType)
		}
	}
}

namespace {
	bool IsObjectType(rttr::type t)
	{
		if(t.is_member_object_pointer() && !t.is_wrapper())
		{
			// if it's a pointer, then we need the type without the pointer.
			rttr::type rawType = t.get_raw_type();
			return rawType.is_derived_from<Mirror::Object>();
		}
		else if(!t.is_member_object_pointer() && t.is_wrapper())
		{
			rttr::type wrappedType = t.get_wrapped_type();
			if(wrappedType.is_pointer())
			{
				rttr::type rawType = wrappedType.get_raw_type();
				return rawType.is_derived_from<Mirror::Object>();
			}
		}
		else if(!t.is_member_object_pointer() && !t.is_wrapper())
		{
			return t.is_derived_from<Mirror::Object>();
		}
		return false;
	}

	bool IsObjectPointer(rttr::type t)
	{
		return t.is_member_object_pointer();
	}

	rttr::type GetRawObjectType(rttr::type t)
	{
		if(t.is_member_object_pointer() && !t.is_wrapper())
		{
			// if it's a pointer, then we need the type without the pointer.
			return t.get_raw_type();
		}
		else if(!t.is_member_object_pointer() && t.is_wrapper())
		{
			rttr::type wrappedType = t.get_wrapped_type();
			if (wrappedType.is_pointer())
			{
				return wrappedType.get_raw_type();
			}
		}
		return t;
	}
}

namespace {
	using namespace rttr;
	bool WriteAtomicTypeToDocument(IDocument* writer, const String& key, const rttr::type& t, const rttr::variant& var)
	{
		if(t.is_arithmetic())
		{
			if (t == type::get<bool>())
				writer->WriteBool(key.c_str(), var.to_bool());
			else if (t == type::get<char>())
				writer->WriteChar(key.c_str(), var.to_bool());

			else if (t == type::get<int8_t>())
				writer->WriteInt8(key.c_str(), var.to_int8());
			else if (t == type::get<uint8_t>())
				writer->WriteInt8(key.c_str(), var.to_uint8());

			else if (t == type::get<int16_t>())
				writer->WriteInt16(key.c_str(), var.to_int16());
			else if (t == type::get<uint16_t>())
				writer->WriteUInt16(key.c_str(), var.to_uint16());

			else if(t == type::get<int32_t>())
				writer->WriteInt32(key.c_str(), var.to_int32());
			else if(t == type::get<uint32_t>())
				writer->WriteUInt32(key.c_str(), var.to_uint32());

			else if (t == type::get<int64_t>())
				writer->WriteInt64(key.c_str(), var.to_int64());
			else if (t == type::get<uint64_t>())
				writer->WriteUInt64(key.c_str(), var.to_uint64());

			else if (t == type::get<float>())
				writer->WriteFloat(key.c_str(), var.to_float());
			else if (t == type::get<double>())
				writer->WriteFloat(key.c_str(), var.to_double());
			return true;
		}
		/*else if(t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if (ok)
			{
				writer->WriteString()
			}
			return true;
		}*/
		else if(t == type::get<String>())
		{
			writer->WriteString(key.c_str(), var.to_string());
			return true;
		}
		return false;
	}
}

void TypeTraits<Mirror::Object*>::RecursiveWrite(IDocument* writer, const String& key, const Mirror::Object* value)
{
	using namespace rttr;

	instance valueInstance(value);

	writer->WriteSubsection(key.c_str());
	writer->EnterSubsection(key.c_str());

	Mirror::Type objType = value->MyType();
	array_range<property> props =
		objType.get_properties(filter_item::instance_item |
                               filter_item::static_item |
                               filter_item::public_access |
                               filter_item::non_public_access);

	for (property prop : props)
	{
		string_view name = prop.get_name();
		if(IsObjectType(prop.get_type()))
		{
			variant childObject(prop.get_value(valueInstance));
			if(childObject.can_convert<Mirror::Object*>())
			{
				TypeTraits<Mirror::Object*>::RecursiveWrite(writer, name.to_string(), childObject.convert<Mirror::Object*>());
			}
			else if(childObject.can_convert<Mirror::Object>())
			{
				Mirror::Object& cObj = childObject.convert<Mirror::Object>();

			}
		}
	}

	writer->LeaveSubsection();
}

Result<void, Error> TypeTraits<Mirror::Object*>::WriteToDocument(IDocument* writer, const String& key, const Mirror::Object* value)
{
	RecursiveWrite(writer, key, value);
	return Result<void, Error>();
}

CLOSE_NAMESPACE(Elf);