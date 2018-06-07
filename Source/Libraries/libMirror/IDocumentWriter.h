///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IDocumentWriter.h
//
//  A writer that wraps a document of some kind (most likely json).
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_IDOCUMENTWRITER_H_
#define LIBMIRROR_IDOCUMENTWRITER_H_
#pragma once

#include <libCore/Result.h>

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocumentWriter
{
public:
	virtual ResultCode WriteChar(const char* key,   const char& inValue)          = 0;
	virtual ResultCode WriteUChar(const char* key,  const unsigned char& inValue) = 0;
	virtual ResultCode WriteInt8(const char* key,   const int8_t& inValue)        = 0;
	virtual ResultCode WriteUInt8(const char* key,  const uint8_t& inValue)       = 0;
	virtual ResultCode WriteInt16(const char* key,  const int16_t& inValue)       = 0;
	virtual ResultCode WriteUInt16(const char* key, const uint16_t& inValue)      = 0;
	virtual ResultCode WriteInt32(const char* key,  const int32_t& inValue)       = 0;
	virtual ResultCode WriteUInt32(const char* key, const uint32_t& inValue)      = 0;
	virtual ResultCode WriteFloat(const char* key,  const float& inValue)         = 0;
	virtual ResultCode WriteDouble(const char* key, const double& inValue)        = 0;
	virtual ResultCode WriteString(const char* key, const string& inValue)        = 0;

	/**
		Write a subsection of the document.
	 **/
	virtual ResultCode WriteSubsection(const char* key) = 0;

	/**
		Enter a subsection of the document.
	 **/
	virtual ResultCode EnterSubsection(const char* section) = 0;

	/**
		Leave a subsection of the document. No-Op if we are already in the root section.
	 **/
	virtual ResultCode LeaveSubsection() = 0;

	/**
		Write to the provided document.
	 **/
	virtual ResultCode WriteToDocument(shared_ptr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif