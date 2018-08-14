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

#include "IDocument.h"
#include <json/json.h>

OPEN_NAMESPACE(Elf);

class IDocument;

class JSONDocument : public IDocument
{
public:
	JSONDocument(const char* data);
	virtual ~JSONDocument();

private:
	virtual Result<bool, Error> ReadBool(const char* key) const;
	virtual Result<char, Error> ReadChar(const char* key) const;
	virtual Result<uchar, Error> ReadUChar(const char* key) const;
	virtual Result<int8_t, Error> ReadInt8(const char* key) const;
	virtual Result<uint8_t, Error> ReadUInt8(const char* key) const;
	virtual Result<int16_t, Error> ReadInt16(const char* key) const;
	virtual Result<uint16_t, Error> ReadUInt16(const char* key) const;
	virtual Result<int32_t, Error> ReadInt32(const char* key) const;
	virtual Result<uint32_t, Error> ReadUInt32(const char* key) const;
	virtual Result<int64_t, Error> ReadInt64(const char* key) const;
	virtual Result<uint64_t, Error> ReadUInt64(const char* key) const;
	virtual Result<float, Error> ReadFloat(const char* key) const;
	virtual Result<double, Error> ReadDouble(const char* key) const;
	virtual Result<String, Error> ReadString(const char* key) const;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const;


	virtual Result<IDocument*, Error> WriteBool(const char* key, const bool& inValue);
	virtual Result<IDocument*, Error> WriteChar(const char* key, const char& inValue);
	virtual Result<IDocument*, Error> WriteUChar(const char* key, const unsigned char& inValue);
	virtual Result<IDocument*, Error> WriteInt8(const char* key, const int8_t& inValue);
	virtual Result<IDocument*, Error> WriteUInt8(const char* key, const uint8_t& inValue);
	virtual Result<IDocument*, Error> WriteInt16(const char* key, const int16_t& inValue);
	virtual Result<IDocument*, Error> WriteUInt16(const char* key, const uint16_t& inValue);
	virtual Result<IDocument*, Error> WriteInt32(const char* key, const int32_t& inValue);
	virtual Result<IDocument*, Error> WriteUInt32(const char* key, const uint32_t& inValue);
	virtual Result<IDocument*, Error> WriteInt64(const char* key, const int64_t& inValue);
	virtual Result<IDocument*, Error> WriteUInt64(const char* key, const uint64_t& inValue);
	virtual Result<IDocument*, Error> WriteFloat(const char* key, const float& inValue);
	virtual Result<IDocument*, Error> WriteDouble(const char* key, const double& inValue);
	virtual Result<IDocument*, Error> WriteString(const char* key, const String& inValue);

	virtual Result<Mirror::Type, Error> GetType(const char* key) const;


	virtual Result<IDocument*, Error> WriteSubsection(const char* key);
	virtual Result<IDocument*, Error> FindSubsection(const char* sectionName);
	virtual Result<IDocument*, Error> EnterSubsection(const char* sectionName);
	virtual Result<IDocument*, Error> LeaveSubsection();

	//virtual Result<IDocument*, Error> WriteToDocument(SharedPtr<IDocument>& document);


	Result<void, Error> SetCursor(const char* section);

private:
	struct Section
	{
		Section();
		void Set(Json::Value* root, const String& section="");

		Result<void, Error> SetPosFromRoot(const std::initializer_list<String>& list);

		bool HasChild(const String& childName) const;

		Result<void, Error>         MoveToChild(const String& toChild = "");
		Result<void, Error>         SetChild(const String& childName, Json::Value& jsonValue);
		Result<Json::Value*, Error> GetChild(const String& childName) const;

		Result<void, Error> MoveToParent();
		Result<void, Error> MoveToSibling(const String& toSibling);

		Result<void, Error> ApplyToDocument();
	private:
		Json::Value*       m_docRoot;       // Root of the document as a whole.
		Json::Value*       m_data;          // Root of the section.
		Json::Value*       m_cursor;        // Current position in the tree.
		List<Json::Value*> m_cursorParents; // Cursor parent stack.
		String             m_sectionName;
	};

	Json::Value m_root; // Root of the document.
	Section m_schema;
	Section m_data;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
};

CLOSE_NAMESPACE(Elf);

#endif