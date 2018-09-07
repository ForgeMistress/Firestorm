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
#include "TypeTraits.h"
#include "IDocument.h"

OPEN_NAMESPACE(Firestorm);

/*const char* TraitsDocs::SCHEMA_KEY = "__FIRE_SCHEMA__";
const char* TraitsDocs::DATA_KEY = "__FIRE_DATA__";
const char* TraitsDocs::OBJECT_TYPE_KEY = "__FIRE_OBJECT_TYPE__";
const char* TraitsDocs::OBJECT_DATA_KEY = "__FIRE_OBJECT_DATA__";

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
	return rttr::type::get<T>();
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
	return FIRE_ERROR(TraitsErrors::READ_ERROR, "could not read 'void' type from document.");
}

Result<void, Error> TypeTraits<void>::WriteToDocument(IDocument*, const String&, void*)
{
	return FIRE_ERROR(TraitsErrors::WRITE_ERROR, "could not write 'void' type to document.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  int8_t specialization
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TRAITS_IMPL(TEMPLATE_TYPE, ABBREVIATION_STRING, FUNCTION_POSTFIX)                                                      \
const char* TypeTraits<TEMPLATE_TYPE>::GetAbbreviation()                                                                       \
{																                                                               \
	static const char* str = ABBREVIATION_STRING;													                           \
	return str;																										           \
}																												               \
const char* TypeTraits<TEMPLATE_TYPE>::GetTypeString()                                                                         \
{																                                                               \
	static const char* str = #TEMPLATE_TYPE;																	               \
	return str;																									               \
}																												               \
const Mirror::Type TypeTraits<TEMPLATE_TYPE>::GetMirrorType() { return Mirror::Type::get<TEMPLATE_TYPE>(); }				   \
Result<void, Error> TypeTraits<TEMPLATE_TYPE>::ReadFromDocument(IDocument* reader, const String& key, TEMPLATE_TYPE& outValue) \
{																												               \
	assert(reader && "document reader is nullptr in Traits<int8_t>::ReadFromDocument.");						               \
	TEMPLATE_TYPE val;																							               \
	Result<void, Error> res = reader->Read##FUNCTION_POSTFIX(key.c_str(), val);									               \
	if (res.has_value())																						               \
	{																											               \
		return Result<void, Error>();																			               \
	}																											               \
	return FIRE_ERROR(TraitsErrors::READ_ERROR, "could not read int8_t from document with key '" + key + "'");	               \
}																												               \
Result<void, Error> TypeTraits<TEMPLATE_TYPE>::WriteToDocument(IDocument* writer, const String& key, TEMPLATE_TYPE value)      \
{																												               \
	assert(writer && "document writer is nullptr in Traits<"#TEMPLATE_TYPE">::WriteToDocument.");				               \
	Result<void, Error> res = writer->Write##FUNCTION_POSTFIX(key.c_str(), value);								               \
	if (res.has_value())																						               \
	{																											               \
		return Result<void, Error>();																			               \
	}																											               \
	return FIRE_ERROR(TraitsErrors::WRITE_ERROR, "could not write 'int8_t' to document with key '" + key + "'");	               \
}

TRAITS_IMPL(int8_t, "i8", Int8)
TRAITS_IMPL(uint8_t, "ui8", UInt8)
TRAITS_IMPL(int16_t, "i16", Int16)
TRAITS_IMPL(uint16_t, "u16", UInt16)
TRAITS_IMPL(int32_t, "i16", Int32)
TRAITS_IMPL(uint32_t, "u16", UInt32)
TRAITS_IMPL(int64_t, "i64", Int64)
TRAITS_IMPL(uint64_t, "u64", UInt64)
TRAITS_IMPL(float, "flt", Float)
TRAITS_IMPL(double, "dbl", Double)
TRAITS_IMPL(String, "str", String)

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
	using namespace rttr;

	Mirror::Type objType = outValue->MyType();
	array_range<property> props = 
		objType.get_properties(filter_item::instance_item | 
                               filter_item::static_item |
			                   filter_item::public_access |
                               filter_item::non_public_access);

	reader->FindSubsection(key.c_str());
	reader->EnterSubsection();

	for(property prop : props)
	{
		variant meta = prop.get_metadata(Mirror::PropMeta::LOAD);
		if(meta.to_bool() == true)
		{
			string_view name = prop.get_name();
			Result<Mirror::Type, Error> propType = reader->GetType(name.to_string().c_str());
			if(propType.has_value())
			{

			}
			//rttr::variant prop
			//rttr::type propType = prop.get_type();
			//if(propType)
		}
	}
	return Result<void, Error>();
}

namespace {
	using namespace rttr;
	bool IsObjectType(type t)
	{
		if(t.is_member_object_pointer() && !t.is_wrapper())
		{
			// if it's a pointer, then we need the type without the pointer.
			type rawType = t.get_raw_type();
			return rawType.is_derived_from<Mirror::Object>();
		}
		else if(!t.is_member_object_pointer() && t.is_wrapper())
		{
			type wrappedType = t.get_wrapped_type();
			if(wrappedType.is_pointer())
			{
				type rawType = wrappedType.get_raw_type();
				return rawType.is_derived_from<Mirror::Object>();
			}
		}
		else if(!t.is_member_object_pointer() && !t.is_wrapper())
		{
			return t.is_derived_from<Mirror::Object>();
		}
		return false;
	}

	bool IsObjectPointer(type t)
	{
		return t.is_member_object_pointer();
	}

	type GetRawObjectType(type t)
	{
		if(t.is_member_object_pointer() && !t.is_wrapper())
		{
			// if it's a pointer, then we need the type without the pointer.
			return t.get_raw_type();
		}
		else if(!t.is_member_object_pointer() && t.is_wrapper())
		{
			type wrappedType = t.get_wrapped_type();
			if (wrappedType.is_pointer())
			{
				return wrappedType.get_raw_type();
			}
		}
		return t;
	}

	bool WriteAtomicTypeToDocument(IDocument* writer, const String& key, const rttr::type& t, const rttr::variant& var)
	{
		if(t.is_arithmetic())
		{
			if(t == type::get<bool>())
				writer->WriteBool(key.c_str(), var.to_bool());
			else if(t == type::get<char>())
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
				writer->WriteDouble(key.c_str(), var.to_double());
			return true;
		}*/
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
		/*else if(t == type::get<String>())
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
	writer->EnterSubsection();
	writer->WriteString("__FIRE_OBJECT_TYPE__", value->MyType().get_name().to_string());

	Mirror::Type objType = value->MyType();
	array_range<property> props =
		objType.get_properties(filter_item::instance_item |
                               filter_item::static_item |
                               filter_item::public_access |
                               filter_item::non_public_access);

	for(property prop : props)
	{
		string_view name = prop.get_name();
		if(IsObjectType(prop.get_type()))
		{
			variant childObject(prop.get_value(valueInstance));
			if(childObject.can_convert<Mirror::Object*>())
			{
				TypeTraits::RecursiveWrite(writer, name.to_string(), childObject.convert<Mirror::Object*>());
			}
			else if(childObject.can_convert<Mirror::Object>())
			{
			}
		}
		else
		{
			bool ok = WriteAtomicTypeToDocument(writer, name.to_string(), prop.get_type().get_raw_type(), prop.get_value(valueInstance));
			if(!ok)
			{
				std::cerr << "error writing property '" << name.to_string() << "'to document" << std::endl;
			}
		}
	}

	writer->LeaveSubsection();
}

Result<void, Error> TypeTraits<Mirror::Object*>::WriteToDocument(IDocument* writer, const String& key, const Mirror::Object* value)
{
	RecursiveWrite(writer, key, value);
	return Result<void, Error>();
}*/

CLOSE_NAMESPACE(Firestorm);