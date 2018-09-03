///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JSONDocument.h
//
//  IDocument implementation that is designed to read and write JSON formatted documents.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBJSON_JSONDOCUMENT_H_
#define LIBJSON_JSONDOCUMENT_H_
#pragma once

#include <libIO/IDocument.h>
#include <json/value.h>

OPEN_NAMESPACE(Elf);

class JSONDocument : public Mirror::Object,
                     public IDocument
{
	ELF_MIRROR_DECLARE(JSONDocument, Mirror::Object, IDocument);
public:
	JSONDocument();
	explicit JSONDocument(const Vector<char>& data);

private:
	virtual bool InitializeAsIfNew();

	virtual Result<void, Error> ReadBool(const char* key, bool& outValue) const;
	virtual Result<void, Error> ReadChar(const char* key, char& outValue) const;
	virtual Result<void, Error> ReadUChar(const char* key, unsigned char& outValue) const;
	virtual Result<void, Error> ReadInt8(const char* key, int8_t& outValue) const;
	virtual Result<void, Error> ReadUInt8(const char* key, uint8_t& outValue) const;
	virtual Result<void, Error> ReadInt16(const char* key, int16_t& outValue) const;
	virtual Result<void, Error> ReadUInt16(const char* key, uint16_t& outValue) const;
	virtual Result<void, Error> ReadInt32(const char* key, int32_t& outValue) const;
	virtual Result<void, Error> ReadUInt32(const char* key, uint32_t& outValue) const;
	virtual Result<void, Error> ReadInt64(const char* key, int64_t& outValue) const;
	virtual Result<void, Error> ReadUInt64(const char* key, uint64_t& outValue) const;
	virtual Result<void, Error> ReadFloat(const char* key, float& outValue) const;
	virtual Result<void, Error> ReadDouble(const char* key, double& outValue) const;
	virtual Result<void, Error> ReadString(const char* key, String& outValue) const;

	virtual Result<Mirror::Type, Error> GetType(const char* key) const;

	virtual Result<void, Error> WriteBool(const char* key, const bool& inValue);
	virtual Result<void, Error> WriteChar(const char* key, const char& inValue);
	virtual Result<void, Error> WriteUChar(const char* key, const unsigned char& inValue);
	virtual Result<void, Error> WriteInt8(const char* key, const int8_t& inValue);
	virtual Result<void, Error> WriteUInt8(const char* key, const uint8_t& inValue);
	virtual Result<void, Error> WriteInt16(const char* key, const int16_t& inValue);
	virtual Result<void, Error> WriteUInt16(const char* key, const uint16_t& inValue);
	virtual Result<void, Error> WriteInt32(const char* key, const int32_t& inValue);
	virtual Result<void, Error> WriteUInt32(const char* key, const uint32_t& inValue);
	virtual Result<void, Error> WriteInt64(const char* key, const int64_t& inValue);
	virtual Result<void, Error> WriteUInt64(const char* key, const uint64_t& inValue);
	virtual Result<void, Error> WriteFloat(const char* key, const float& inValue);
	virtual Result<void, Error> WriteDouble(const char* key, const double& inValue);
	virtual Result<void, Error> WriteString(const char* key, const String& inValue);

	virtual Result<void, Error> WriteSubsection(const char* key);
	virtual Result<void, Error> FindSubsection(const char* sectionName);
	virtual Result<void, Error> EnterSubsection();
	virtual Result<void, Error> LeaveSubsection();

	virtual void WriteToData(Vector<char>& outData) const;
	virtual String ToString() const;


	Result<void, Error> SetCursor(const char* section);

private:
	struct Section
	{
		Section();
		void Set(Json::Value* root);

		Result<void, Error> SetPosFromRoot(const std::initializer_list<String>& list);

		bool HasChild(const String& childName) const;

		Result<void, Error>         MoveToChild(const String& toChild = "");
		Result<void, Error>         SetChild(const String& childName, Json::Value& jsonValue);
		Result<Json::Value*, Error> GetChild(const String& childName) const;

		const Json::Value& GetValue(const String& name) const;
		Json::ValueType GetJsonType(const String& name) const;

		Result<void, Error> MoveToParent();
	private:
		Json::Value*       m_docRoot;       // Root of the document as a whole.
		Json::Value*       m_cursor;        // Current position in the tree.
		List<Json::Value*> m_cursorParents; // Cursor parent stack.
		String             m_sectionName;
	};

	Json::Value m_root; // Root of the document.
	Section m_data;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
};

CLOSE_NAMESPACE(Elf);

#endif