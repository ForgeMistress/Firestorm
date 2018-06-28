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

	virtual Result<char>      ReadChar(const char* key)   const = 0;
	virtual Result<uchar>     ReadUChar(const char* key)  const = 0;
	virtual Result<int8_t>    ReadInt8(const char* key)   const = 0;
	virtual Result<uint8_t>   ReadUInt8(const char* key)  const = 0;
	virtual Result<int16_t>   ReadInt16(const char* key)  const = 0;
	virtual Result<uint16_t>  ReadUInt16(const char* key) const = 0;
	virtual Result<int32_t>   ReadInt32(const char* key)  const = 0;
	virtual Result<uint32_t>  ReadUInt32(const char* key) const = 0;
	virtual Result<float>     ReadFloat(const char* key)  const = 0;
	virtual Result<double>    ReadDouble(const char* key) const = 0;
	virtual Result<String>    ReadString(const char* key) const = 0;

	virtual Result<Mirror::Type> GetType(const char* key) const = 0;

	virtual Result<IDocumentReader*> FindSubsection(const char* sectionName) = 0;
	virtual Result<IDocumentReader*> EnterSubsection(const char* sectionName) = 0;
	virtual Result<IDocumentReader*> LeaveSubsection() = 0;

	virtual Result<IDocumentReader*> ReadDocument(SharedPtr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif