////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SerialTraits.h
//
//  This contains the templatized type traits object that is used throughout the engine. This in particular
//  focuses on serialization and deserialization.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_SERIALTRAITS_H_
#define LIBMIRROR_SERIALTRAITS_H_
#pragma once

#include <libCore/Result.h>
#include <json/value.h>


OPEN_NAMESPACE(Firestorm);

class IDocument;

/**
	Convert a Json::Value to an rttr::variant.
 **/
extern rttr::variant Convert(const Json::Value& value);

/**
	Convert an rttr::variant to a Json::Value.
 **/
extern Json::Value Convert(const rttr::variant& value);

OPEN_NAMESPACE(Mirror);


CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Firestorm);

#endif