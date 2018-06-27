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
#include "JSONDocumentReader.h"
#include "TypeAbbreviationTraits.h"

OPEN_NAMESPACE(Elf);

JSONDocumentReader::JSONDocumentReader()
{
}

JSONDocumentReader::~JSONDocumentReader()
{
}

ResultCode JSONDocumentReader::ReadChar(const char* key, char& outValue) const
{
	return Result::OK;
}

ResultCode JSONDocumentReader::ReadUChar(const char* key, unsigned char& outValue) const
{
	return Result::OK;
}

ResultCode JSONDocumentReader::ReadInt8(const char* key, int8_t& outValue) const
{
	return Result::OK;
}

ResultCode JSONDocumentReader::ReadUInt8(const char* key, uint8_t& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadInt16(const char* key, int16_t& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadUInt16(const char* key, uint16_t& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadInt32(const char* key, int32_t& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadUInt32(const char* key, uint32_t& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadFloat(const char* key, float& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadDouble(const char* key, double& outValue) const
{
	return Result::OK;
}
ResultCode JSONDocumentReader::ReadString(const char* key, String& outValue) const
{
	return Result::OK;
}

Mirror::Type JSONDocumentReader::GetType(const char* key) const
{
	return rttr::detail::get_invalid_type();
}

ResultCode JSONDocumentReader::FindSubsection(const char* sectionName)
{
	assert(m_dataSection.cursor != nullptr && m_typeSection.cursor != nullptr);
	if(m_dataSection.cursor->isMember(sectionName))
	{
		m_foundSubsection = sectionName;
		return Result::OK;
	}
	return IDocumentReader::ERR_SUBSECTION_NOT_FOUND;
}

ResultCode JSONDocumentReader::EnterSubsection(const char* sectionName)
{
	ResultCode rc = FindSubsection(sectionName);
	if(rc.IsOK())
	{
		m_dataSecti
	}
	return rc;
}

ResultCode JSONDocumentReader::LeaveSubsection()
{
	return Result::OK;
}

ResultCode JSONDocumentReader::ReadDocument(SharedPtr<IDocument>& document)
{
	return Result::OK;
}

void JSONDocumentReader::SetCursor(const char* section)
{

}

/*private:
	struct Section
	{
		Json::Value  data;
		Json::Value* cursor;
	};
	Json::Value m_root; // Root of the document.
	Section m_dataSection;
	Section m_typeSection;

	String m_foundSubsection; // If FindSection is called, then this will be filled if it's found.
*/

CLOSE_NAMESPACE(Elf);