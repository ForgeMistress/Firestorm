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


OPEN_NAMESPACE(Firestorm);


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


CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Firestorm);