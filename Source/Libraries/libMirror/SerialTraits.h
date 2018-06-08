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
typedef shared_ptr<IDocumentWriter> IDocumentWriterPtr;

class IDocumentReader;
typedef shared_ptr<IDocumentReader> IDocumentReaderPtr;

OPEN_NAMESPACE(Mirror);

class Object;

/**
	SerialTraits

	Template type that can be used to read data from and write data to documents.
 **/
template <class T>
struct SerialTraits
{
	static const Result& Write(const char* key, IDocumentWriterPtr& writer, const T& input);
	static const Result& Read(const char* key, IDocumentReaderPtr& reader, T& output);
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

template<>
struct SerialTraits<Object*>
{
	typedef Object* T;
	static ResultCode Write(const char* key, IDocumentWriterPtr& writer, const T& input);
	static ResultCode Read(const char* key, IDocumentReaderPtr& writer, T& output);
};

template<>
struct SerialTraits<shared_ptr<Object> >
{
	typedef shared_ptr<Object> T;
	static ResultCode Write(const char* key, IDocumentWriterPtr& writer, const T& input);
	static ResultCode Read(const char* key, IDocumentReaderPtr& reader, T& output);
};

template<>
struct SerialTraits<const Object*>
{
	typedef const Object* T;
	static ResultCode Write(const char* key, IDocumentWriterPtr& writer, const T& input);
	static ResultCode Read(const char* key, IDocumentReaderPtr& reader, T& output)
	{
		assert(false && "Object is WriteOnly.");
	}
};

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);

#endif