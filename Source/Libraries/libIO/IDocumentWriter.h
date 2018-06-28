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
	virtual Result<IDocumentWriter*> WriteChar(const char* key,   const char& inValue)          = 0;
	virtual Result<IDocumentWriter*> WriteUChar(const char* key,  const unsigned char& inValue) = 0;
	virtual Result<IDocumentWriter*> WriteInt8(const char* key,   const int8_t& inValue)        = 0;
	virtual Result<IDocumentWriter*> WriteUInt8(const char* key,  const uint8_t& inValue)       = 0;
	virtual Result<IDocumentWriter*> WriteInt16(const char* key,  const int16_t& inValue)       = 0;
	virtual Result<IDocumentWriter*> WriteUInt16(const char* key, const uint16_t& inValue)      = 0;
	virtual Result<IDocumentWriter*> WriteInt32(const char* key,  const int32_t& inValue)       = 0;
	virtual Result<IDocumentWriter*> WriteUInt32(const char* key, const uint32_t& inValue)      = 0;
	virtual Result<IDocumentWriter*> WriteFloat(const char* key,  const float& inValue)         = 0;
	virtual Result<IDocumentWriter*> WriteDouble(const char* key, const double& inValue)        = 0;
	virtual Result<IDocumentWriter*> WriteString(const char* key, const String& inValue)        = 0;

	virtual Result<Mirror::Type> GetType(const char* key) const = 0;

	/**
		Write a subsection of the document.
	 **/
	virtual Result<IDocumentWriter*> WriteSubsection(const char* key) = 0;

	/**
		Enter a subsection of the document.
	 **/
	virtual Result<IDocumentWriter*> EnterSubsection(const char* section) = 0;

	/**
		Leave a subsection of the document. No-Op if we are already in the root section.
	 **/
	virtual Result<IDocumentWriter*> LeaveSubsection() = 0;

	/**
		Write to the provided document.
	 **/
	virtual Result<IDocumentWriter*> WriteToDocument(SharedPtr<IDocument>& document) = 0;
};

CLOSE_NAMESPACE(Elf);

#endif