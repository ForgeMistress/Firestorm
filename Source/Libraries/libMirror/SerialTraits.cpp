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
#include <libIO/IDocument.h>

#define DEFINE_POD_SERIAL_TRAITS(TYPE, FUNCTYPE)                                                               \
Result<void, Error> SerialTraits<TYPE>::Write(const char* key, RefPtr<IDocument> writer, const TYPE& input) \
{																											   \
	Result<void, Error> rc = writer->Write##FUNCTYPE(key, input);											   \
	if (!rc.has_value()) {																					   \
		return ELF_ERROR(SerialResults::WRITE_ERROR, rc.error().Message);									   \
	}																										   \
	return Result<void, Error>();																			   \
}																											   \
Result<void, Error> SerialTraits<TYPE>::Read(const char* key, RefPtr<IDocument> reader, TYPE& output)	   \
{																											   \
	return reader->Read##FUNCTYPE(key, output);																   \
}

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

template <class T>
inline Result<RefPtr<IDocument>, Error> SerialTraits<T>::Write(const char* key, RefPtr<IDocument> writer, const T& input)
{
	static_assert(false, "Default SerialTraits has not been implemented.");
	return error(SerialResults::NOT_IMPLEMENTED, "Default SerialTraits has not been implemented.");
}

template <class T>
inline Result<T, Error> SerialTraits<T>::Read(const char* key, RefPtr<IDocument> reader, T& output)
{
	static_assert(false, "Default SerialTraits has not been implemented.");
	return error(SerialResults::NOT_IMPLEMENTED, "Default SerialTraits has not been implemented.");
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
Result<void, Error> SerialTraits<Object*>::Write(const char* key, RefPtr<IDocument> writer, const T& input)
{
	return SerialTraits<const Object*>::Write(key, writer, input);
}

Result<void, Error> SerialTraits<Object*>::Read(const char* key, RefPtr<IDocument> reader, T& output)
{
	return Result<void,Error>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<const Object*>
Result<void, Error> SerialTraits<const Object*>::Write(const char* key, RefPtr<IDocument> writer, const T& input)
{
	using namespace rttr;
	//Mirror::Type objType = input->get_type();
	return Result<void,Error>();
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

Result<void,Error> SerialTraits<const Object*>::Read(const char* key, RefPtr<IDocument> reader, T& input)
{
	assert(false && "const Object* is Write-Only.");
	return ELF_ERROR(SerialResults::INPUT_NOT_VALID, "input is not valid for this operation");
	//tl::make_unexpected<Error>(Error(SerialResults::INPUT_NOT_VALID, "input is not valid for this operation"));
}

#define _onfailedreturn(tt, op) { \
	ResultCode<IDocument*> rc = op ; \
	if(!rc.has_value()) { \
		return ELF_ERROR(SerialResults::WRITE_ERROR, rc.error()); \
	} \
}

#define _classtypekey "__ELF_CLASS_TYPE__"
#define _classdatakey "__ELF_CLASS_INSTANCE_DATA__"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits< SharedPtr<Object> >
Result<void,Error> SerialTraits<RefPtr<Object>>::Write(const char* key, RefPtr<IDocument> writer, const RefPtr<Object>& input)
{
	Mirror::Type objType = input->GetType();

	assert(objType.is_valid());

	/*_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteSubsection(key));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->EnterSubsection(key));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteString(_classtypekey, objType.get_name().to_string()));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteSubsection(_classdatakey));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->EnterSubsection(_classdatakey));*/
	using namespace std::placeholders;
	auto r = writer->WriteSubsection(key);
		/*.and_then(std::bind(&IDocumentWriter::EnterSubsection, _1, key))
			.map_error([=](const Error& err) {
				throw std::exception(err.Message.c_str());
			})
		.and_then(std::bind(&IDocumentWriter::WriteString, _1, _classtypekey, objType.get_name().to_string()))
			.map_error([=](const Error& err) {
				throw std::exception(err.Message.c_str());
			})
		.and_then(std::bind(&IDocumentWriter::WriteSubsection, _1, _classdatakey))
			.map_error([=](const Error& err) {
				throw std::exception(err.Message.c_str());
			})
		.and_then(std::bind(&IDocumentWriter::EnterSubsection, _1, _classdatakey))
			.map_error([=](const Error& err) {
				throw std::exception(err.Message.c_str());
			})
		;*/

	if(r.has_value())
	{
		rttr::instance inputInstance(input);

		for (auto prop : objType.get_properties())
		{
			rttr::variant propMeta = prop.get_metadata(Mirror::PropMeta::SAVE);
			if (propMeta.is_type<bool>() && propMeta.get_value<bool>())
			{
				const char* propName = prop.get_name().to_string().c_str();
				rttr::variant propValue(prop.get_value(inputInstance));

				Result<void,Error> rc = SerialTraits<rttr::variant>::Write(propName, writer, propValue);
				if (!rc.has_value())
				{
					return rc;
				}
			}
		}

		writer->LeaveSubsection();
		writer->LeaveSubsection();
	}
	//_onfailedreturn(writer->LeaveSubsection()); // _classdatakey
	//_onfailedreturn(writer->LeaveSubsection()); // key

	return Result<void,Error>();
}

Result<void,Error> SerialTraits< RefPtr<Object> >::Read(const char* key, RefPtr<IDocument> reader, RefPtr<Object>& output)
{
	Mirror::Type objType = output->GetType();

	assert(objType.is_valid());

	//String outputClassType;
	/*_onfailedreturn(reader->FindSubsection(key));
	_onfailedreturn(reader->EnterSubsection(key));
	_onfailedreturn(reader->ReadString(_classtypekey, outputClassType));

	assert(objType.get_name().to_string() == outputClassType && "Class type mismatch.");

	_onfailedreturn(reader->FindSubsection(_classdatakey));
	_onfailedreturn(reader->EnterSubsection(_classdatakey));

	rttr::instance outputInstance(output);
	for(auto prop : objType.get_properties())
	{
		rttr::variant propLoadMeta = prop.get_metadata(Mirror::PropMeta::kLoad);
		if(propLoadMeta.is_type<bool>() && propLoadMeta.get_value<bool>())
		{
			const char* propName = prop.get_name().to_string().c_str();

			
		}
	}*/

	return Result<void,Error>();
}

#define _writeiftype(ty) if(input.is_type<ty>()) { return SerialTraits<ty>::Write(key, writer, input.get_value<ty>()); }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<rttr::variant>
Result<void,Error> SerialTraits<rttr::variant>::Write(const char* key, RefPtr<IDocument> writer, const rttr::variant& input)
{
	if(!input.is_valid()) { 
		return ELF_ERROR(SerialResults::VARIANT_NOT_VALID, "variant " + String(key) + " is not valid");
	}
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
	return Result<void, Error>();
}
#undef _writeiftype

Result<void, Error> SerialTraits<rttr::variant>::Read(const char* key, RefPtr<IDocument> reader, rttr::variant& output)
{
	return Result<void,Error>();
}

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);