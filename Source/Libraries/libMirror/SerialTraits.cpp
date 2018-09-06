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

//#define DEFINE_POD_SERIAL_TRAITS(TYPE, FUNCTYPE)                                                               \
Result<void, Error> SerialTraits<TYPE>::Write(const char* key, Json::Value& writer, const TYPE& input) \
{																											   \
	writer[key] = input;\
	return Result<void, Error>();																			   \
}																											   \
Result<void, Error> SerialTraits<TYPE>::Read(const char* key, const Json::Value& reader, TYPE& output)	   \
{																											   \	
	//rttr::type t = rttr::type::get<TYPE>();\
	if(t.is_valid())\
	{\
		rttr::property\
	}\
}

OPEN_NAMESPACE(Elf);


/**
	Convert a Json::Value to an rttr::variant.
 **/
rttr::variant Convert(const Json::Value& value)
{
	switch(value.type())
	{
	case Json::nullValue:
		return rttr::variant();
	case Json::intValue:
		return rttr::variant(value.asInt());
	case Json::uintValue:
		return rttr::variant(value.asUInt());
	case Json::realValue:
		return rttr::variant(value.asDouble());
	case Json::stringValue:
		return rttr::variant(value.asString());
	case Json::booleanValue:
		return rttr::variant(value.asBool());
	}
	throw std::exception("could not convert Json::Value to rttr::variant");
	return rttr::variant();
}

/**
	Convert an rttr::variant to a Json::Value.
 **/
Json::Value Convert(const rttr::variant& var)
{
	using namespace rttr;
	type t = var.get_type();
	if(t.is_arithmetic())
	{
		if(t == type::get<bool>())
			return Json::Value(var.to_bool());
		else if(t == type::get<char>())
			return Json::Value(var.to_bool());
		else if(t == type::get<int8_t>())
			return Json::Value(var.to_int8());
		else if(t == type::get<int16_t>())
			return Json::Value(var.to_int16());
		else if(t == type::get<int32_t>())
			return Json::Value(var.to_int32());
		else if(t == type::get<int64_t>())
			return Json::Value(var.to_int64());
		else if(t == type::get<uint8_t>())
			return Json::Value(var.to_uint8());
		else if(t == type::get<uint16_t>())
			return Json::Value(var.to_uint16());
		else if(t == type::get<uint32_t>())
			return Json::Value(var.to_uint32());
		else if(t == type::get<uint64_t>())
			return Json::Value(var.to_uint64());
		else if(t == type::get<float>())
			return Json::Value(var.to_double());
		else if(t == type::get<double>())
			return Json::Value(var.to_double());
	}
	else if(t == type::get<String>())
	{
		return Json::Value(var.to_string());
	}
	throw std::exception("could not convert rttr::variant to Json::Value");
	return Json::Value::nullSingleton();
}


OPEN_NAMESPACE(Mirror);

template <class T>
inline Result<void, Error> SerialTraits<T>::Write(const char* key, Json::Value& writer, const T& input)
{
	static_assert(false, "Default SerialTraits has not been implemented.");
	return ELF_ERROR(SerialResults::NOT_IMPLEMENTED, "Default SerialTraits has not been implemented.");
}

template <class T>
inline Result<void, Error> SerialTraits<T>::Read(const char* key, const Json::Value& reader, T& output)
{
	static_assert(false, "Default SerialTraits has not been implemented.");
	return ELF_ERROR(SerialResults::NOT_IMPLEMENTED, "Default SerialTraits has not been implemented.");
}

Result<void, Error> SerialTraits<int8_t>::Write(const char* key, Json::Value& output, const int8_t& input)
{																										
	output[key] = input;
	return Result<void, Error>();																			
}																										
Result<void, Error> SerialTraits<int8_t>::Read(const char* key, const Json::Value& input, int8_t& output)
{													
	output = input[key].asInt();
	return Result<void, Error>();
}

/*DEFINE_POD_SERIAL_TRAITS(int8_t,   Int8);
DEFINE_POD_SERIAL_TRAITS(uint8_t,  UInt8);
DEFINE_POD_SERIAL_TRAITS(int16_t,  Int16);
DEFINE_POD_SERIAL_TRAITS(uint16_t, UInt16);
DEFINE_POD_SERIAL_TRAITS(int32_t,  Int32);
DEFINE_POD_SERIAL_TRAITS(uint32_t, UInt32);
DEFINE_POD_SERIAL_TRAITS(float,    Float);
DEFINE_POD_SERIAL_TRAITS(double,   Double);
DEFINE_POD_SERIAL_TRAITS(String,   String);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<Object*>
Result<void, Error> SerialTraits<Object*>::Write(const char* key, Json::Value& output, const T& input)
{
	return SerialTraits<const Object*>::Write(key, output, input);
}

Result<void, Error> SerialTraits<Object*>::Read(const char* key, const Json::Value& input, T& output)
{
	return Result<void,Error>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<const Object*>
Result<void, Error> SerialTraits<const Object*>::Write(const char* key, Json::Value& output, const T& input)
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
/*
Result<void,Error> SerialTraits<const Object*>::Read(const char* key, const Json::Value& output, T& input)
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
Result<void,Error> SerialTraits<RefPtr<Object>>::Write(const char* key, Json::Value& output, const RefPtr<Object>& input)
{
	Mirror::Type objType = input->GetType();

	assert(objType.is_valid());
	*/
	/*_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteSubsection(key));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->EnterSubsection(key));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteString(_classtypekey, objType.get_name().to_string()));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->WriteSubsection(_classdatakey));
	_onfailedreturn(SharedPtr<IDocumentWriter>, writer->EnterSubsection(_classdatakey));*/
//sing namespace std::placeholders;
	//auto r = writer->WriteSubsection(key);
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

	/*if(r.has_value())
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
	}*/
	//_onfailedreturn(writer->LeaveSubsection()); // _classdatakey
	//_onfailedreturn(writer->LeaveSubsection()); // key
	/*

	return Result<void,Error>();
}

Result<void,Error> SerialTraits< RefPtr<Object> >::Read(const char* key, const Json::Value& reader, RefPtr<Object>& output)
{
	Mirror::Type objType = output->GetType();

	assert(objType.is_valid());
	*/
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
/*
	return Result<void,Error>();
}

#define _writeiftype(ty) if(input.is_type<ty>()) { return SerialTraits<ty>::Write(key, writer, input.get_value<ty>()); }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialTraits<rttr::variant>
Result<void,Error> SerialTraits<rttr::variant>::Write(const char* key, Json::Value& output, const rttr::variant& input)
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
	/*return Result<void, Error>();
}
#undef _writeiftype

Result<void, Error> SerialTraits<rttr::variant>::Read(const char* key, const Json::Value& input, rttr::variant& output)
{
	return Result<void,Error>();
}*/

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);