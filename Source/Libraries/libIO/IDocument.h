///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IDocument.h
//
//  A document.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_IDOCUMENT_H_
#define LIBIO_IDOCUMENT_H_
#pragma once

#include <libCore/Result.h>

OPEN_NAMESPACE(Elf);

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocument
{
	ELF_MIRROR_DECLARE(IDocument);
public:
	virtual ~IDocument() {}

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

	virtual bool InitializeAsIfNew() = 0;

	virtual Result<void, Error> ReadBool(const char* key, bool& outValue) const = 0;
	virtual Result<void, Error> ReadChar(const char* key, char& outValue) const = 0;
	virtual Result<void, Error> ReadUChar(const char* key, unsigned char& outValue) const = 0;
	virtual Result<void, Error> ReadInt8(const char* key, int8_t& outValue) const = 0;
	virtual Result<void, Error> ReadUInt8(const char* key, uint8_t& outValue) const = 0;
	virtual Result<void, Error> ReadInt16(const char* key, int16_t& outValue) const = 0;
	virtual Result<void, Error> ReadUInt16(const char* key, uint16_t& outValue) const = 0;
	virtual Result<void, Error> ReadInt32(const char* key, int32_t& outValue) const = 0;
	virtual Result<void, Error> ReadUInt32(const char* key, uint32_t& outValue) const = 0;
	virtual Result<void, Error> ReadInt64(const char* key, int64_t& outValue) const = 0;
	virtual Result<void, Error> ReadUInt64(const char* key, uint64_t& outValue) const = 0;
	virtual Result<void, Error> ReadFloat(const char* key, float& outValue) const = 0;
	virtual Result<void, Error> ReadDouble(const char* key, double& outValue) const = 0;
	virtual Result<void, Error> ReadString(const char* key, String& outValue) const = 0;

	virtual Result<void, Error> WriteSubsection(const char* key) = 0;
	virtual Result<void, Error> FindSubsection(const char* sectionName) = 0;
	virtual Result<void, Error> EnterSubsection() = 0;
	virtual Result<void, Error> LeaveSubsection() = 0;

	virtual Result<void, Error> WriteBool(const char* key, const bool& inValue) = 0;
	virtual Result<void, Error> WriteChar(const char* key, const char& inValue) = 0;
	virtual Result<void, Error> WriteUChar(const char* key, const unsigned char& inValue) = 0;
	virtual Result<void, Error> WriteInt8(const char* key, const int8_t& inValue) = 0;
	virtual Result<void, Error> WriteUInt8(const char* key, const uint8_t& inValue) = 0;
	virtual Result<void, Error> WriteInt16(const char* key, const int16_t& inValue) = 0;
	virtual Result<void, Error> WriteUInt16(const char* key, const uint16_t& inValue) = 0;
	virtual Result<void, Error> WriteInt32(const char* key, const int32_t& inValue) = 0;
	virtual Result<void, Error> WriteUInt32(const char* key, const uint32_t& inValue) = 0;
	virtual Result<void, Error> WriteInt64(const char* key, const int64_t& inValue) = 0;
	virtual Result<void, Error> WriteUInt64(const char* key, const uint64_t& inValue) = 0;
	virtual Result<void, Error> WriteFloat(const char* key, const float& inValue) = 0;
	virtual Result<void, Error> WriteDouble(const char* key, const double& inValue) = 0;
	virtual Result<void, Error> WriteString(const char* key, const String& inValue) = 0;

	virtual void WriteToData(Vector<char>& outData) const = 0;
	virtual String ToString() const = 0;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const = 0;

	enum Metadata : uint8_t
	{
		MAKER_FUNCTION_BLANK,
		MAKER_FUNCTION_DATA
	};
	static rttr::detail::metadata MakerFunctionBlank() { return rttr::metadata(MAKER_FUNCTION_BLANK, true); }
	static rttr::detail::metadata MakerFunctionData()  { return rttr::metadata(MAKER_FUNCTION_DATA, true); }
};

CLOSE_NAMESPACE(Elf);

#endif