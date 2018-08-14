///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IDocumentReader.h
//
//  A result of an operation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_IDOCUMENT_H_
#define LIBMIRROR_IDOCUMENT_H_
#pragma once

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocument
{
public:
	enum Codes : uint8_t
	{
		ERROR, // generic
		READ_DOCUMENT_FAILED,
		READ_VALUE_FAILED,
		SUBSECTION_NOT_FOUND,
		FIND_SUBSECTION_FAILED,
		ENTER_SUBSECTION_FAILED,
		GET_TYPE_FAILED
	};

	virtual Result<bool, Error>      ReadBool(const char* key)   const = 0;
	virtual Result<char, Error>      ReadChar(const char* key)   const = 0;
	virtual Result<uchar, Error>     ReadUChar(const char* key)  const = 0;
	virtual Result<int8_t, Error>    ReadInt8(const char* key)   const = 0;
	virtual Result<uint8_t, Error>   ReadUInt8(const char* key)  const = 0;
	virtual Result<int16_t, Error>   ReadInt16(const char* key)  const = 0;
	virtual Result<uint16_t, Error>  ReadUInt16(const char* key) const = 0;
	virtual Result<int32_t, Error>   ReadInt32(const char* key)  const = 0;
	virtual Result<uint32_t, Error>  ReadUInt32(const char* key) const = 0;
	virtual Result<int64_t, Error>   ReadInt64(const char* key)  const = 0;
	virtual Result<uint64_t, Error>  ReadUInt64(const char* key) const = 0;
	virtual Result<float, Error>     ReadFloat(const char* key)  const = 0;
	virtual Result<double, Error>    ReadDouble(const char* key) const = 0;
	virtual Result<String, Error>    ReadString(const char* key) const = 0;

	virtual Result<IDocument*, Error> WriteSubsection(const char* key) = 0;
	virtual Result<IDocument*, Error> FindSubsection(const char* sectionName) = 0;
	virtual Result<IDocument*, Error> EnterSubsection(const char* sectionName) = 0;
	virtual Result<IDocument*, Error> LeaveSubsection() = 0;

	virtual Result<IDocument*, Error> WriteBool(const char* key, const bool& inValue) = 0;
	virtual Result<IDocument*, Error> WriteChar(const char* key, const char& inValue) = 0;
	virtual Result<IDocument*, Error> WriteUChar(const char* key, const unsigned char& inValue) = 0;
	virtual Result<IDocument*, Error> WriteInt8(const char* key, const int8_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteUInt8(const char* key, const uint8_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteInt16(const char* key, const int16_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteUInt16(const char* key, const uint16_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteInt32(const char* key, const int32_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteUInt32(const char* key, const uint32_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteInt64(const char* key, const int64_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteUInt64(const char* key, const uint64_t& inValue) = 0;
	virtual Result<IDocument*, Error> WriteFloat(const char* key, const float& inValue) = 0;
	virtual Result<IDocument*, Error> WriteDouble(const char* key, const double& inValue) = 0;
	virtual Result<IDocument*, Error> WriteString(const char* key, const String& inValue) = 0;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const = 0;
};

CLOSE_NAMESPACE(Elf);

#endif