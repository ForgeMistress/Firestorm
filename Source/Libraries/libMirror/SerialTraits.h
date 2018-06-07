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

#define DECLARE_POD_SERIAL_TRAITS(TYPE) \
	template<> \
	class SerialTraits<TYPE> \
	{ \
	public: \
		static const Result& Write(const char* key, shared_ptr<IDocumentWriter>& writer, const TYPE& output); \
		static const Result& Read(const char* key, shared_ptr<IDocumentReader>& reader, TYPE& output); \
	}

OPEN_NAMESPACE(Elf);

class IDocumentWriter;
class IDocumentReader;

OPEN_NAMESPACE(Mirror);

class Object;

template <class T>
struct SerialTraits
{
	static const Result& Write(const char* key, shared_ptr<IDocumentWriter>& writer, const T& input);
	static const Result& Read(const char* key, shared_ptr<IDocumentReader>& reader, T& output);
};

DECLARE_POD_SERIAL_TRAITS(int8_t);
DECLARE_POD_SERIAL_TRAITS(uint8_t);
DECLARE_POD_SERIAL_TRAITS(int16_t);
DECLARE_POD_SERIAL_TRAITS(uint16_t);
DECLARE_POD_SERIAL_TRAITS(int32_t);
DECLARE_POD_SERIAL_TRAITS(uint32_t);
DECLARE_POD_SERIAL_TRAITS(float);
DECLARE_POD_SERIAL_TRAITS(double);
DECLARE_POD_SERIAL_TRAITS(string);

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif
