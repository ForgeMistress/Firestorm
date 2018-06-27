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

#include <libCore/Result.h>

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocumentReader
{
public:
	static ResultCode ERR_SUBSECTION_NOT_FOUND;

	virtual ResultCode ReadChar(const char* key,   char& outValue)          const = 0;
	virtual ResultCode ReadUChar(const char* key,  unsigned char& outValue) const = 0;
	virtual ResultCode ReadInt8(const char* key,   int8_t& outValue)        const = 0;
	virtual ResultCode ReadUInt8(const char* key,  uint8_t& outValue)       const = 0;
	virtual ResultCode ReadInt16(const char* key,  int16_t& outValue)       const = 0;
	virtual ResultCode ReadUInt16(const char* key, uint16_t& outValue)      const = 0;
	virtual ResultCode ReadInt32(const char* key,  int32_t& outValue)       const = 0;
	virtual ResultCode ReadUInt32(const char* key, uint32_t& outValue)      const = 0;
	virtual ResultCode ReadFloat(const char* key,  float& outValue)         const = 0;
	virtual ResultCode ReadDouble(const char* key, double& outValue)        const = 0;
	virtual ResultCode ReadString(const char* key, String& outValue)        const = 0;

	virtual Mirror::Type GetType(const char* key) const = 0;

	virtual ResultCode FindSubsection(const char* sectionName) = 0;
	virtual ResultCode EnterSubsection(const char* sectionName) = 0;
	virtual ResultCode LeaveSubsection() = 0;

	virtual ResultCode ReadDocument(SharedPtr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif