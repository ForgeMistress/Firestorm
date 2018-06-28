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
	virtual Result<char> ReadChar(const char* key)            const;
	virtual Result<uchar> ReadUChar(const char* key) const;
	virtual Result<int8_t> ReadInt8(const char* key)        const;
	virtual Result<uint8_t> ReadUInt8(const char* key)     const;
	virtual Result<int16_t> ReadInt16(const char* key)     const;
	virtual Result<uint16_t> ReadUInt16(const char* key)  const;
	virtual Result<int32_t> ReadInt32(const char* key)     const;
	virtual Result<uint32_t> ReadUInt32(const char* key)  const;
	virtual Result<float> ReadFloat(const char* key)         const;
	virtual Result<double> ReadDouble(const char* key)      const;
	virtual Result<String> ReadString(const char* key)      const;

	virtual Result<Mirror::Type> GetType(const char* key) const;

	virtual Result<IDocumentReader*> FindSubsection(const char* sectionName);
	virtual Result<IDocumentReader*> EnterSubsection(const char* sectionName);
	virtual Result<IDocumentReader*> LeaveSubsection();

	virtual Result<IDocumentReader*> ReadDocument(SharedPtr<IDocument>& document);

	Result<void> SetCursor(const char* section);

private:
	struct Section
	{
		Json::Value  data;                // Root of the section
		Json::Value* cursor;              // Current position in the tree.
		List<Json::Value*> cursorParents; // Cursor parent stack.
	};
	Json::Value m_root; // Root of the document.
	Section m_dataSection;
	Section m_typeSection;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
};

CLOSE_NAMESPACE(Elf);

#endif