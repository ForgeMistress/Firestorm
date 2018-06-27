///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JSONDocumentReader.h
//
//  IDocumentReader implementation that is designed to read JSON formatted documents.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_JSONDOCUMENTREADER_H_
#define LIBIO_JSONDOCUMENTREADER_H_
#pragma once

#include <libCore/Result.h>

#include "IDocumentReader.h"
#include <json/json.h>

OPEN_NAMESPACE(Elf);

class IDocument;

class JSONDocumentReader : public IDocumentReader
{
public:
	JSONDocumentReader();
	virtual ~JSONDocumentReader();

private:
	virtual ResultCode ReadChar(const char* key, char& outValue)           const;
	virtual ResultCode ReadUChar(const char* key, unsigned char& outValue) const;
	virtual ResultCode ReadInt8(const char* key, int8_t& outValue)         const;
	virtual ResultCode ReadUInt8(const char* key, uint8_t& outValue)       const;
	virtual ResultCode ReadInt16(const char* key, int16_t& outValue)       const;
	virtual ResultCode ReadUInt16(const char* key, uint16_t& outValue)     const;
	virtual ResultCode ReadInt32(const char* key, int32_t& outValue)       const;
	virtual ResultCode ReadUInt32(const char* key, uint32_t& outValue)     const;
	virtual ResultCode ReadFloat(const char* key, float& outValue)         const;
	virtual ResultCode ReadDouble(const char* key, double& outValue)       const;
	virtual ResultCode ReadString(const char* key, String& outValue)       const;

	virtual Mirror::Type GetType(const char* key) const;

	virtual ResultCode FindSubsection(const char* sectionName);
	virtual ResultCode EnterSubsection(const char* sectionName);
	virtual ResultCode LeaveSubsection();

	virtual ResultCode ReadDocument(SharedPtr<IDocument>& document);

	void SetCursor(const char* section);

private:
	struct Section
	{
		Json::Value  data;
		Json::Value* cursor;
	};
	Json::Value m_root; // Root of the document.
	Section m_dataSection;
	Section m_typeSection;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
};

CLOSE_NAMESPACE(Elf);

#endif