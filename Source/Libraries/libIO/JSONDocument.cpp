///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JSONDocumentReader.cpp
//
//  IDocumentReader implementation that is designed to read JSON formatted documents.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "JSONDocument.h"
#include "TypeAbbreviationTraits.h"
#include <sstream>
#include <json/reader.h>

OPEN_NAMESPACE(Elf);

JSONDocument::JSONDocument(const char* data)
{
	if(data != nullptr)
	{
		Json::Reader reader;
		bool ok = reader.parse(String(data), m_root, false);
		if(!ok)
		{
			std::cerr << reader.getFormattedErrorMessages() << std::endl;
		}
		else
		{
			m_data.Set(&m_root, "__ELF_DATA__");
			m_schema.Set(&m_root, "__ELF_SCHEMA__");
		}
	}
	else
	{
		m_data.
		m_data.SetChild("__ELF_DATA__", Json::Value(Json::objectValue));
		m_schema.SetChild("__ELF_SCHEMA__", Json::Value(Json::objectValue));
	}
}

JSONDocument::~JSONDocument()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// READ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Result<bool, Error> JSONDocument::ReadBool(const char* key) const
{
	return Result<bool, Error>(false);
}

Result<char, Error> JSONDocument::ReadChar(const char* key) const
{
	return Result<char, Error>(0);
}

Result<uchar, Error> JSONDocument::ReadUChar(const char* key) const
{
	return Result<uchar, Error>(0);
}

Result<int8_t, Error> JSONDocument::ReadInt8(const char* key) const
{
	return Result<int8_t, Error>(0);
}

Result<uint8_t, Error> JSONDocument::ReadUInt8(const char* key) const
{
	return Result<uint8_t, Error>(0);
}

Result<int16_t, Error> JSONDocument::ReadInt16(const char* key) const
{
	return Result<int16_t, Error>(0);
}

Result<uint16_t, Error> JSONDocument::ReadUInt16(const char* key) const
{
	return Result<uint16_t, Error>(0);
}

Result<int32_t, Error> JSONDocument::ReadInt32(const char* key) const
{
	return Result<int32_t, Error>(0);
}

Result<uint32_t, Error> JSONDocument::ReadUInt32(const char* key) const
{
	return Result<uint32_t, Error>(0);
}

Result<int64_t, Error> JSONDocument::ReadInt64(const char* key) const
{
	return Result<int64_t, Error>(0);
}

Result<uint64_t, Error> JSONDocument::ReadUInt64(const char* key) const
{
	return Result<uint64_t, Error>(0);
}

Result<float, Error> JSONDocument::ReadFloat(const char* key) const
{
	return Result<float, Error>(0.0f);
}

Result<double, Error> JSONDocument::ReadDouble(const char* key) const
{
	return Result<double, Error>(0.0);
}
Result<String, Error> JSONDocument::ReadString(const char* key) const
{
	return Result<String, Error>(String(""));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WRITE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Result<IDocument*, Error> JSONDocument::WriteBool(const char* key, const bool& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteChar(const char* key, const char& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteUChar(const char* key, const unsigned char& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteInt8(const char* key, const int8_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteUInt8(const char* key, const uint8_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteInt16(const char* key, const int16_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteUInt16(const char* key, const uint16_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteInt32(const char* key, const int32_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteUInt32(const char* key, const uint32_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteInt64(const char* key, const int64_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteUInt64(const char* key, const uint64_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteFloat(const char* key, const float& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteDouble(const char* key, const double& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::WriteString(const char* key, const String& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	m_schema.SetChild(key, Json::Value(TypeAbbreviations::getStr(inValue)));
	return nullptr;
}

Result<Mirror::Type, Error> JSONDocument::GetType(const char* key) const
{
	/*Result<Json::Value*, Error> res = m_schema.GetChild(key);
	if (res.has_value())
	{
		Json::Value* val = res.value();
		assert(val != nullptr);
		Json::ValueType valType = val->type();
		switch (valType)
		{
		case Json::nullValue:
		case Json::intValue:
		case Json::uintValue:
		case Json::realValue:
		case Json::stringValue:
		case Json::booleanValue:
		case Json::arrayValue:
		case Json::objectValue:
			return TypeTraits<Mirror::Object>::GetMirrorType();
		}
	}
	TypeTraits<int16_t>::WriteToDocument(const_cast<JSONDocumentWriter*>(this), key, 10);*/
	return Result<Mirror::Type, Error>(rttr::detail::get_invalid_type());
}


Result<IDocument*, Error> JSONDocument::WriteSubsection(const char* key)
{
	return nullptr;
}

Result<IDocument*, Error> JSONDocument::FindSubsection(const char* sectionName)
{
	/*assert(m_dataSection.cursor != nullptr && m_typeSection.cursor != nullptr);
	if(m_dataSection.cursor->isMember(sectionName) && m_typeSection.cursor->isMember(sectionName))
	{
	m_foundSubsection = sectionName;
	return result(this);
	}
	std::stringstream ss;
	ss << "subsection " << sectionName << " was not found.";*/
	return Result<IDocument*, Error>(nullptr); //error(FIND_SUBSECTION_FAILED, "");
}

Result<IDocument*, Error> JSONDocument::EnterSubsection(const char* sectionName)
{
	Result<IDocument*, Error> findss = FindSubsection(sectionName);
	if (findss.has_value())
	{
		auto setcursor = SetCursor(sectionName);
		if (setcursor.has_value())
		{
			return Result<IDocument*, Error>(this);
		}
		return ELF_ERROR(ENTER_SUBSECTION_FAILED, "failed to enter subsection " + String(sectionName));
	}
	return findss;
}

Result<IDocument*, Error> JSONDocument::LeaveSubsection()
{
	return nullptr;
}

Result<void, Error> JSONDocument::SetCursor(const char* section)
{
	if(m_schema.HasChild(section) && m_data.HasChild(section))
	{
		m_schema.MoveToChild(section);
		m_data.MoveToChild(section);
	}
	return Result<void, Error>();
}


JSONDocument::Section::Section()
: m_docRoot(nullptr)
, m_data(nullptr)
, m_cursor(nullptr)
{
}

void JSONDocument::Section::Set(Json::Value* root, const String& section)
{
	if (section != "")
	{
		m_cursorParents.clear();

		m_docRoot = root;
		assert(m_docRoot->isMember(section) && "Member does not exist.");
		m_data = &(*m_docRoot)[section];
		m_cursor = m_data;
	}
	else
	{
		m_docRoot = root;
		m_data = m_docRoot;
		m_cursor = m_data;
	}
}

Result<void, Error> JSONDocument::Section::SetPosFromRoot(const std::initializer_list<String>& list)
{
	return Result<void, Error>();
}

bool JSONDocument::Section::HasChild(const String& childName) const
{
	return m_cursor->isMember(childName);
}

Result<void, Error> JSONDocument::Section::MoveToChild(const String& toChild)
{

	return Result<void, Error>();
}

Result<void, Error> JSONDocument::Section::SetChild(const String& childName, Json::Value& jsonValue)
{
	(*m_cursor)[childName] = jsonValue;
}

Result<Json::Value*, Error> JSONDocument::Section::GetChild(const String& childName) const
{
	if (m_cursor->isMember(childName))
	{
		return &(*m_cursor)[childName];
	}
}

Result<void, Error> JSONDocument::Section::MoveToParent()
{
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::Section::MoveToSibling(const String& toSibling)
{
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::Section::ApplyToDocument()
{
	return Result<void, Error>();
}

CLOSE_NAMESPACE(Elf);