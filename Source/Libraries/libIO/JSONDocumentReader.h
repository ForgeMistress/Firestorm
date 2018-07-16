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
	virtual Result<char, Error> ReadChar(const char* key) const;
	virtual Result<uchar, Error> ReadUChar(const char* key) const;
	virtual Result<int8_t, Error> ReadInt8(const char* key) const;
	virtual Result<uint8_t, Error> ReadUInt8(const char* key) const;
	virtual Result<int16_t, Error> ReadInt16(const char* key) const;
	virtual Result<uint16_t, Error> ReadUInt16(const char* key) const;
	virtual Result<int32_t, Error> ReadInt32(const char* key) const;
	virtual Result<uint32_t, Error> ReadUInt32(const char* key) const;
	virtual Result<float, Error> ReadFloat(const char* key) const;
	virtual Result<double, Error> ReadDouble(const char* key) const;
	virtual Result<String, Error> ReadString(const char* key) const;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const;

	virtual Result<IDocumentReader*, Error> FindSubsection(const char* sectionName);
	virtual Result<IDocumentReader*, Error> EnterSubsection(const char* sectionName);
	virtual Result<IDocumentReader*, Error> LeaveSubsection();

	virtual Result<IDocumentReader*, Error> ReadDocument(SharedPtr<IDocument>& document);

	Result<void, Error> SetCursor(const char* section);

private:
	struct Section
	{
		Section() : m_cursor(nullptr) {}

		Result<void, Error> SetPosFromRoot(const std::initializer_list<String>& list);
		Result<void, Error> MoveToChild(const String& toChild = "");
		Result<void, Error> MoveToParent();
		Result<void, Error> MoveToSibling(const String& toSibling);
	private:
		Json::Value  m_data;                // Root of the section
		Json::Value* m_cursor;              // Current position in the tree.
		List<Json::Value*> m_cursorParents; // Cursor parent stack.
	};
	Json::Value m_root; // Root of the document.
	Section m_dataSection;
	Section m_typeSection;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
};

CLOSE_NAMESPACE(Elf);

#endif