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

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

#include <libIO/TypeTraits.h>

#include <libMirror/Object.h>

#include <json/reader.h>

#include <sstream>
#include <istream>

OPEN_NAMESPACE(Firestorm);

namespace {
	static RefPtr<IDocument> MakeBlankJSON()
	{
		return new JSONDocument();
	}

	static RefPtr<IDocument> MakeDataJSON(const Vector<char>& data)
	{
		return new JSONDocument(data);
	}
}

FIRE_MIRROR_DEFINE_NAMED(JSONDocument, "Firestorm::Document::JSON")
{
	Class.method("MakeBlank", &MakeBlankJSON)
	(
		IDocument::MakerFunctionBlank()
	);

	Class.method("MakeData", &MakeDataJSON)
	(
		IDocument::MakerFunctionData()
	);

	Class.method("InitializeAsIfNew", &JSONDocument::InitializeAsIfNew);
}

JSONDocument::JSONDocument()
{
	InitializeAsIfNew();
}

JSONDocument::JSONDocument(const Vector<char>& data)
{
	if(!data.empty())
	{
		Json::CharReaderBuilder rbuilder;
		String errs;
		std::istringstream stream(&data[0]);
		bool ok = Json::parseFromStream(rbuilder, stream, &m_root, &errs);
		if(!ok)
		{
			std::cerr << errs << std::endl;
		}
		else
		{
			m_data.Set(&m_root);
		}
	}
	else
	{
		m_root = Json::Value(Json::objectValue);
		m_data.Set(&m_root);
	}
}

bool JSONDocument::InitializeAsIfNew()
{
	m_root = Json::Value(Json::objectValue);
	m_data.Set(&m_root);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// READ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Result<void, Error> JSONDocument::ReadBool(const char* key, bool& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asBool();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadChar(const char* key, char& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadUChar(const char* key, unsigned char& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadInt8(const char* key, int8_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadUInt8(const char* key, uint8_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asUInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadInt16(const char* key, int16_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadUInt16(const char* key, uint16_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asUInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadInt32(const char* key, int32_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadUInt32(const char* key, uint32_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asUInt();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadInt64(const char* key, int64_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asInt64();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadUInt64(const char* key, uint64_t& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asUInt64();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadFloat(const char* key, float& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asFloat();
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::ReadDouble(const char* key, double& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asDouble();
	}
	return Result<void, Error>();
}
Result<void, Error> JSONDocument::ReadString(const char* key, String& outValue) const
{
	if(m_data.HasChild(key))
	{
		const Json::Value& val = m_data.GetValue(key);
		outValue = val.asString();
	}
	return Result<void, Error>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WRITE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Result<void, Error> JSONDocument::WriteBool(const char* key, const bool& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteChar(const char* key, const char& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteUChar(const char* key, const unsigned char& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteInt8(const char* key, const int8_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteUInt8(const char* key, const uint8_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteInt16(const char* key, const int16_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteUInt16(const char* key, const uint16_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteInt32(const char* key, const int32_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteUInt32(const char* key, const uint32_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteInt64(const char* key, const int64_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteUInt64(const char* key, const uint64_t& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteFloat(const char* key, const float& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteDouble(const char* key, const double& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::WriteString(const char* key, const String& inValue)
{
	m_data.SetChild(key, Json::Value(inValue));
	return Result<void, Error>();
}

Result<Mirror::Type, Error> JSONDocument::GetType(const char* key) const
{
	return Result<Mirror::Type, Error>(rttr::detail::get_invalid_type());
}


Result<void, Error> JSONDocument::WriteSubsection(const char* key)
{
	m_data.SetChild(key, Json::Value(Json::objectValue));
	m_foundSubsection = key;
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::FindSubsection(const char* sectionName)
{
	if(m_data.HasChild(sectionName))
	{
		Json::ValueType dType = m_data.GetJsonType(sectionName);
		FIRE_ASSERT(dType == Json::objectValue);
		m_foundSubsection = sectionName;
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::EnterSubsection()
{
	if(m_foundSubsection.empty())
	{
		return FIRE_ERROR(IDocument::ENTER_SUBSECTION_FAILED, "no subsection was found before attempting to enter");
	}
	m_data.MoveToChild(m_foundSubsection);
	m_foundSubsection.clear();
	return Result<void, Error>();
}

Result<void, Error> JSONDocument::LeaveSubsection()
{
	m_data.MoveToParent();
	return Result<void, Error>();
}

void JSONDocument::WriteToData(Vector<char>& outData) const
{
	outData.clear();

	//Json::StyledWriter writer;
	Json::FastWriter writer;
	String result(writer.write(m_root));
	outData.reserve(result.size());
	std::copy(result.c_str(), result.c_str() + result.length(), std::back_inserter(outData));
	outData.shrink_to_fit();
	//std::copy(result.begin(), result.end(), outData);
}

String JSONDocument::ToString() const
{
	Json::FastWriter writer;
	return writer.write(m_root);
}

Result<void, Error> JSONDocument::SetCursor(const char* section)
{
	if(m_data.HasChild(section))
	{
		m_data.MoveToChild(section);
	}
	return Result<void, Error>();
}


JSONDocument::Section::Section()
: m_docRoot(nullptr)
, m_cursor(nullptr)
{
}

void JSONDocument::Section::Set(Json::Value* root)
{
	m_cursorParents.clear();

	m_docRoot = root;
	m_cursor = m_docRoot;
}

Result<void, Error> JSONDocument::Section::SetPosFromRoot(const std::initializer_list<String>& list)
{
	return Result<void, Error>();
}

bool JSONDocument::Section::HasChild(const String& childName) const
{
	FIRE_ASSERT(m_cursor != nullptr);
	return m_cursor->isMember(childName);
}

Result<void, Error> JSONDocument::Section::MoveToChild(const String& toChild)
{
	FIRE_ASSERT(m_cursor != nullptr);
	if(m_cursor->isMember(toChild))
	{
		m_cursorParents.push_back(m_cursor);
		m_cursor = &(*m_cursor)[toChild];
	}

	return Result<void, Error>();
}

Result<void, Error> JSONDocument::Section::SetChild(const String& childName, Json::Value& jsonValue)
{
	FIRE_ASSERT(m_cursor != nullptr);
	(*m_cursor)[childName] = jsonValue;
	return Result<void, Error>();
}

Result<Json::Value*, Error> JSONDocument::Section::GetChild(const String& childName) const
{
	FIRE_ASSERT(m_cursor != nullptr);
	if (m_cursor->isMember(childName))
	{
		return &(*m_cursor)[childName];
	}
	return Result<Json::Value*, Error>();
}

const Json::Value& JSONDocument::Section::GetValue(const String& name) const
{
	FIRE_ASSERT(m_cursor != nullptr);
	return (*m_cursor)[name];
}

Json::ValueType JSONDocument::Section::GetJsonType(const String& name) const
{
	FIRE_ASSERT(m_cursor != nullptr);
	return (*m_cursor)[name].type();
}

Result<void, Error> JSONDocument::Section::MoveToParent()
{
	FIRE_ASSERT(!m_cursorParents.empty());
	m_cursor = m_cursorParents.back();
	m_cursorParents.pop_back();
	return Result<void, Error>();
}

CLOSE_NAMESPACE(Firestorm);