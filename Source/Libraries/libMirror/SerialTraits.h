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
		static const Result& Write(const char* key, SharedPtr<IDocumentWriter>& writer, const OBJ_T& input); \
		static const Result& Read(const char* key, SharedPtr<IDocumentReader>& reader, OBJ_T& output); \
	}

#define DECLARE_OBJ_SERIAL_TRAITS(OBJ_T) \
	template<> \
	struct SerialTraits< OBJ_T > \
	{ \
		typedef OBJ_T T; \
		static ResultCode Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input); \
		static ResultCode Read(const char* key, SharedPtr<IDocumentReader>& writer, T& output); \
	}

OPEN_NAMESPACE(Elf);

class IDocumentWriter;
class IDocumentReader;

OPEN_NAMESPACE(Mirror);

class Object;

/**
	SerialTraits

	Template type that can be used to read data from and write data to documents.
 **/
template <class T>
struct SerialTraits
{
	static const Result& Write(const char* key, SharedPtr<IDocumentWriter>& writer, const T& input);
	static const Result& Read(const char* key, SharedPtr<IDocumentReader>& reader, T& output);
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

DECLARE_OBJ_SERIAL_TRAITS(Object);
DECLARE_OBJ_SERIAL_TRAITS(Object*);
DECLARE_OBJ_SERIAL_TRAITS(const Object*);
DECLARE_OBJ_SERIAL_TRAITS(SharedPtr<Object>);

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif