////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SerialTraits.cpp
//
//  This contains the templatized type traits object that is used throughout the engine. This in particular
//  focuses on serialization and deserialization.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SerialTraits.h"

#include "Object.h"

#include <libIO/File.h>
#include <libIO/IDocumentReader.h>
#include <libIO/IDocumentWriter.h>

#define DEFINE_POD_SERIAL_TRAITS(TYPE, FUNCTYPE) \
const Result& SerialTraits<TYPE>::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const TYPE& input) \
{ \
	return writer -> Write##FUNCTYPE (key, input); \
} \
const Result& SerialTraits<TYPE>::Read(const char* key, SharedPtr<IDocumentReader>& reader, TYPE& output) \
{ \
	return reader -> Read##FUNCTYPE (key, output); \
}

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

const ResultCode SerialResults::RES_VARIANT_NOT_VALID = 
	Result("Variant passed into SerialTraits<>::Write is not valid and stores no data.");

template <class T>
inline const Result& SerialTraits<T>::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input)
{
	static_assert("Default SerialTraits has not been implemented.");
	return Result::ERROR;
}

template <class T>
inline const Result& SerialTraits<T>::Read(const char* key, SharedPtr<IDocumentReader>& reader, T& output)
{
	static_assert("Default SerialTraits has not been implemented.");
	return Result::ERROR;
}

DEFINE_POD_SERIAL_TRAITS(int8_t,   Int8);
DEFINE_POD_SERIAL_TRAITS(uint8_t,  UInt8);
DEFINE_POD_SERIAL_TRAITS(int16_t,  Int16);
DEFINE_POD_SERIAL_TRAITS(uint16_t, UInt16);
DEFINE_POD_SERIAL_TRAITS(int32_t,  Int32);
DEFINE_POD_SERIAL_TRAITS(uint32_t, UInt32);
DEFINE_POD_SERIAL_TRAITS(float,    Float);
DEFINE_POD_SERIAL_TRAITS(double,   Double);
DEFINE_POD_SERIAL_TRAITS(String,   String);

/*
std::string testStringValue("testing value!");
std::shared_ptr<TestObjectWithFields> test = std::make_shared<TestObjectWithFields>();
test->testString = testStringValue;

instance objInstance(test);
Elf::Mirror::Type testType = test->GetType();

property testStringProperty = testType.get_property("testString");
variant extractedValueVariant = testStringProperty.get_value(objInstance);

t.Assert(extractedValueVariant.get_value<std::string>() == testStringValue, "testStringValue != extractedValueVariant");
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<Object*>
ResultCode SerialTraits<Object*>::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input)
{
	return SerialTraits<const Object*>::Write(key, writer, input);
}

ResultCode SerialTraits<Object*>::Read(const char* key, SharedPtr<IDocumentReader>& reader, T& output)
{
	return Result::OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<const Object*>
ResultCode SerialTraits<const Object*>::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input)
{
	using namespace rttr;
	//Mirror::Type objType = input->get_type();
	return Result::OK;
}
	/*using namespace rttr;
	Mirror::Type objType = input->GetType();

	instance inputInstance(input);
	inputInstance.get_wrapped_instance

	array_range<property> properties = objType.get_properties();
	for (auto prop : properties)
	{
		variant propMeta = prop.get_metadata(Mirror::PropMeta::kSave);
		if (propMeta.get_value<bool>())
		{
			const char* propName = prop.get_name().to_string().c_str();
			variant propValue(prop.get_value(inputInstance));

			ResultCode rc = SerialTraits<rttr::variant>::Write(propName, writer, propValue);
			if(rc.IsNotOK())
			{
				return rc;
			}
		}
	}
	return Result::OK;
}*/

ResultCode SerialTraits<const Object*>::Read(const char* key, SharedPtr<IDocumentReader>& writer, T& input)
{
	assert(false && "const Object* is Write-Only.");
	return Result::OK;
}

#define _onfailedreturn(op) { ResultCode rc = op ; if(rc.IsNotOK()) { return rc; } }
#define _classtypekey "__ELF_CLASS_TYPE__"
#define _classdatakey "__ELF_CLASS_INSTANCE_DATA__"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits< SharedPtr<Object> >
ResultCode SerialTraits< SharedPtr<Object> >::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input)
{
	using namespace rttr;
	Mirror::Type objType = input->GetType();

	assert(objType.is_valid());

	_onfailedreturn(writer->WriteSubsection(key));
	_onfailedreturn(writer->EnterSubsection(key));
	_onfailedreturn(writer->WriteString(_classtypekey, key));
	_onfailedreturn(writer->WriteSubsection(_classdatakey));
	_onfailedreturn(writer->EnterSubsection(_classdatakey));

	instance inputInstance(input);

	array_range<property> properties = objType.get_properties();
	for(auto prop : properties)
	{
		variant propMeta = prop.get_metadata(Mirror::PropMeta::kSave);
		if(propMeta.get_value<bool>())
		{
			const char* propName = prop.get_name().to_string().c_str();
			variant propValue(prop.get_value(inputInstance));

			ResultCode rc = SerialTraits<rttr::variant>::Write(propName, writer, propValue);
			if(rc.IsNotOK())
			{
				return rc;
			}
		}
	}

	_onfailedreturn(writer->LeaveSubsection()); // _classdatakey
	_onfailedreturn(writer->LeaveSubsection()); // key

	return Result::OK;
}

ResultCode SerialTraits< SharedPtr<Object> >::Read(const char* key, SharedPtr<IDocumentReader>& reader, T& output)
{
	return Result::OK;
}

#define _writeiftype(ty) if(input.is_type<ty>()) { return SerialTraits<ty>::Write(key, writer, input.get_value<ty>()); }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<rttr::variant>
ResultCode SerialTraits<rttr::variant>::Write(const char* key, SharedPtr<IDocumentWriter>& writer, const rttr::variant& input)
{
	if(!input.is_valid()) { return SerialResults::RES_VARIANT_NOT_VALID; }
	/*_writeiftype(int8_t);
	_writeiftype(uint8_t);
	_writeiftype(int16_t);
	_writeiftype(uint16_t);
	_writeiftype(int32_t);
	_writeiftype(uint32_t);
	_writeiftype(float);
	_writeiftype(double);
	_writeiftype(String);
	_writeiftype(Object*);
	_writeiftype(SharedPtr<Object>);*/
	return Result::OK;
}
#undef _writeiftype

ResultCode SerialTraits<rttr::variant>::Read(const char* key, SharedPtr<IDocumentReader>& reader, rttr::variant& output)
{
	return Result::OK;
}

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);