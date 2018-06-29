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

OPEN_NAMESPACE(Elf);

class IDocument;

OPEN_NAMESPACE(Mirror);
class Object;
CLOSE_NAMESPACE(Mirror);

class IDocumentWriter
{
public:
	virtual Result<IDocumentWriter*, Error> WriteChar(const char* key,   const char& inValue)          = 0;
	virtual Result<IDocumentWriter*, Error> WriteUChar(const char* key,  const unsigned char& inValue) = 0;
	virtual Result<IDocumentWriter*, Error> WriteInt8(const char* key,   const int8_t& inValue)        = 0;
	virtual Result<IDocumentWriter*, Error> WriteUInt8(const char* key,  const uint8_t& inValue)       = 0;
	virtual Result<IDocumentWriter*, Error> WriteInt16(const char* key,  const int16_t& inValue)       = 0;
	virtual Result<IDocumentWriter*, Error> WriteUInt16(const char* key, const uint16_t& inValue)      = 0;
	virtual Result<IDocumentWriter*, Error> WriteInt32(const char* key,  const int32_t& inValue)       = 0;
	virtual Result<IDocumentWriter*, Error> WriteUInt32(const char* key, const uint32_t& inValue)      = 0;
	virtual Result<IDocumentWriter*, Error> WriteFloat(const char* key,  const float& inValue)         = 0;
	virtual Result<IDocumentWriter*, Error> WriteDouble(const char* key, const double& inValue)        = 0;
	virtual Result<IDocumentWriter*, Error> WriteString(const char* key, const String& inValue)        = 0;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const = 0;

	/**
		Write a subsection of the document.
	 **/
	virtual Result<IDocumentWriter*, Error> WriteSubsection(const char* key) = 0;

	/**
		Enter a subsection of the document.
	 **/
	virtual Result<IDocumentWriter*, Error> EnterSubsection(const char* section) = 0;

	/**
		Leave a subsection of the document. No-Op if we are already in the root section.
	 **/
	virtual Result<IDocumentWriter*, Error> LeaveSubsection() = 0;

	/**
		Write to the provided document.
	 **/
	virtual Result<IDocumentWriter*, Error> WriteToDocument(SharedPtr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif