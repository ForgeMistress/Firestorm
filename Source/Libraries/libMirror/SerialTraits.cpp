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
#include "IFile.h"
#include "IDocumentReader.h"
#include "IDocumentWriter.h"

#define DEFINE_POD_SERIAL_TRAITS(TYPE, FUNCTYPE) \
	const Result& SerialTraits<TYPE>::Write(const char* key, std::shared_ptr<IDocumentWriter>& writer, const TYPE& input) \
	{ \
		return writer -> Write##FUNCTYPE (key, input); \
	} \
	const Result& SerialTraits<TYPE>::Read(const char* key, std::shared_ptr<IDocumentReader>& reader, TYPE& output) \
	{ \
		return reader -> Read##FUNCTYPE (key, output); \
	}

OPEN_NAMESPACE(Elf);
OPEN_NAMESPACE(Mirror);

template <class T>
inline const Result& SerialTraits<T>::Write(const char* key, shared_ptr<IDocumentWriter>& writer, const T& input)
{
	static_assert("Default SerialTraits has not been implemented.");
	return Result::ERROR;
}

template <class T>
inline const Result& SerialTraits<T>::Read(const char* key, shared_ptr<IDocumentReader>& reader, T& output)
{
	static_assert("Default SerialTraits has not been implemented.");
	return Result::ERROR;
}

DEFINE_POD_SERIAL_TRAITS(int8_t, Int8);
DEFINE_POD_SERIAL_TRAITS(uint8_t, UInt8);
DEFINE_POD_SERIAL_TRAITS(int16_t, Int16);
DEFINE_POD_SERIAL_TRAITS(uint16_t, UInt16);
DEFINE_POD_SERIAL_TRAITS(int32_t, Int32);
DEFINE_POD_SERIAL_TRAITS(uint32_t, UInt32);
DEFINE_POD_SERIAL_TRAITS(float, Float);
DEFINE_POD_SERIAL_TRAITS(double, Double);
DEFINE_POD_SERIAL_TRAITS(string, String);

CLOSE_NAMESPACE(Mirror);
CLOSE_NAMESPACE(Elf);