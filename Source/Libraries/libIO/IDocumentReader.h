///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IDocumentReader.h
//
//  A result of an operation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_IDOCUMENTREADER_H_
#define LIBMIRROR_IDOCUMENTREADER_H_
#pragma once

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocumentReader
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

	virtual Result<char, Error>      ReadChar(const char* key)   const = 0;
	virtual Result<uchar, Error>     ReadUChar(const char* key)  const = 0;
	virtual Result<int8_t, Error>    ReadInt8(const char* key)   const = 0;
	virtual Result<uint8_t, Error>   ReadUInt8(const char* key)  const = 0;
	virtual Result<int16_t, Error>   ReadInt16(const char* key)  const = 0;
	virtual Result<uint16_t, Error>  ReadUInt16(const char* key) const = 0;
	virtual Result<int32_t, Error>   ReadInt32(const char* key)  const = 0;
	virtual Result<uint32_t, Error>  ReadUInt32(const char* key) const = 0;
	virtual Result<float, Error>     ReadFloat(const char* key)  const = 0;
	virtual Result<double, Error>    ReadDouble(const char* key) const = 0;
	virtual Result<String, Error>    ReadString(const char* key) const = 0;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const = 0;

	virtual Result<IDocumentReader*, Error> FindSubsection(const char* sectionName) = 0;
	virtual Result<IDocumentReader*, Error> EnterSubsection(const char* sectionName) = 0;
	virtual Result<IDocumentReader*, Error> LeaveSubsection() = 0;

	virtual Result<IDocumentReader*, Error> ReadDocument(SharedPtr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif