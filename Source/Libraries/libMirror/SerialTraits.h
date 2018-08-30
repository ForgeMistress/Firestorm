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

#define DECLARE_POD_SERIAL_TRAITS(OBJ_T) \
	template<> \
	class SerialTraits<OBJ_T> \
	{ \
	public: \
		static Result<void, Error> Write(const char* key, RefPtr<IDocument> writer, const OBJ_T& input); \
		static Result<void, Error> Read(const char* key, RefPtr<IDocument> reader, OBJ_T& output); \
	}

#define DECLARE_OBJ_SERIAL_TRAITS(OBJ_T) \
	template<> \
	struct SerialTraits< OBJ_T > \
	{ \
		typedef OBJ_T T; \
		static Result<void, Error> Write(const char* key, RefPtr<IDocument> writer, const T& input); \
		static Result<void, Error> Read(const char* key, RefPtr<IDocument> reader, T& output); \
	}

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);

class Object;

struct SerialResults
{
	enum Codes
	{
		NOT_IMPLEMENTED,
		INPUT_NOT_VALID,
		VARIANT_NOT_VALID,
		WRITE_ERROR
	};
};

/**
	SerialTraits

	Template type that can be used to read data from and write data to documents.
 **/
template <class T>
struct SerialTraits
{
	static Result<RefPtr<IDocument>, Error> Write(const char* key, RefPtr<IDocument> writer, const T& input);
	static Result<T, Error> Read(const char* key, RefPtr<IDocument> reader, T& output);
};

DECLARE_POD_SERIAL_TRAITS(int8_t);
DECLARE_POD_SERIAL_TRAITS(uint8_t);
DECLARE_POD_SERIAL_TRAITS(int16_t);
DECLARE_POD_SERIAL_TRAITS(uint16_t);
DECLARE_POD_SERIAL_TRAITS(int32_t);
DECLARE_POD_SERIAL_TRAITS(uint32_t);
DECLARE_POD_SERIAL_TRAITS(float);
DECLARE_POD_SERIAL_TRAITS(double);
DECLARE_POD_SERIAL_TRAITS(String);
DECLARE_POD_SERIAL_TRAITS(rttr::variant);

DECLARE_OBJ_SERIAL_TRAITS(Object);
DECLARE_OBJ_SERIAL_TRAITS(Object*);
DECLARE_OBJ_SERIAL_TRAITS(const Object*);
DECLARE_OBJ_SERIAL_TRAITS(RefPtr<Object>);

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif