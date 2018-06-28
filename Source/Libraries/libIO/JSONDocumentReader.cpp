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
#include <sstream>

OPEN_NAMESPACE(Elf);

JSONDocumentReader::JSONDocumentReader()
{
	m_dataSection.cursor = nullptr;
	m_typeSection.cursor = nullptr;
}

JSONDocumentReader::~JSONDocumentReader()
{
}

Result<char> JSONDocumentReader::ReadChar(const char* key) const
{
	return result(0);
}

Result<uchar> JSONDocumentReader::ReadUChar(const char* key) const
{
	return result(0);
}

Result<int8_t> JSONDocumentReader::ReadInt8(const char* key) const
{
	return result(0);
}

Result<uint8_t> JSONDocumentReader::ReadUInt8(const char* key) const
{
	return result(0);
}

Result<int16_t> JSONDocumentReader::ReadInt16(const char* key) const
{
	return result(0);
}

Result<uint16_t> JSONDocumentReader::ReadUInt16(const char* key) const
{
	return result(0);
}

Result<int32_t> JSONDocumentReader::ReadInt32(const char* key) const
{
	return result(0);
}

Result<uint32_t> JSONDocumentReader::ReadUInt32(const char* key) const
{
	return result(0);
}

Result<float> JSONDocumentReader::ReadFloat(const char* key) const
{
	return result(0.0f);
}

Result<double> JSONDocumentReader::ReadDouble(const char* key) const
{
	return result(0.0);
}
Result<String> JSONDocumentReader::ReadString(const char* key) const
{
	return result(String(""));
}

Result<Mirror::Type> JSONDocumentReader::GetType(const char* key) const
{
	// Check the type section.
	if(m_typeSection.cursor == nullptr)
	{
		return error(GET_TYPE_FAILED, "type section cursor is invalid");
	}

	Json::Value* typeCursor = m_typeSection.cursor;
	Json::Value* dataCursor = m_dataSection.cursor;
	// If both sections have the member, then deduce the type of the member.
	if(typeCursor->isMember(key) && dataCursor->isMember(key))
	{
		if(!(*typeCursor)[key].isString())
		{
			return error(GET_TYPE_FAILED, "value stored in type section under key '" + String(key) + "' is invalid");
		}
		String typeonfile((*typeCursor)[key].asString());

	}

	return result(rttr::detail::get_invalid_type());
}

Result<IDocumentReader*> JSONDocumentReader::FindSubsection(const char* sectionName)
{
	assert(m_dataSection.cursor != nullptr && m_typeSection.cursor != nullptr);
	if(m_dataSection.cursor->isMember(sectionName) && m_typeSection.cursor->isMember(sectionName))
	{
		m_foundSubsection = sectionName;
		return result(this);
	}
	std::stringstream ss;
	ss << "subsection " << sectionName << " was not found.";
	return error(FIND_SUBSECTION_FAILED, ss.str());
}

Result<IDocumentReader*> JSONDocumentReader::EnterSubsection(const char* sectionName)
{
	Result<IDocumentReader*> findss = FindSubsection(sectionName);
	if (findss.has_value())
	{
		auto setcursor = SetCursor(sectionName);
		if(setcursor.has_value())
		{
			return result(this);
		}
		return error(ENTER_SUBSECTION_FAILED, "failed to enter subsection " + String(sectionName));
	}
	return findss;
}

Result<IDocumentReader*> JSONDocumentReader::LeaveSubsection()
{
	return result(this);
}

Result<IDocumentReader*> JSONDocumentReader::ReadDocument(SharedPtr<IDocument>& document)
{
	return result(this);
}

Result<void> JSONDocumentReader::SetCursor(const char* section)
{
	if(m_foundSubsection == section)
	{
		Json::Value* dataCursor = m_dataSection.cursor;    // Store the current pos
		m_dataSection.cursor = &(*dataCursor)[section];    // Set current pos to enter the child
		m_dataSection.cursorParents.push_back(dataCursor); // Store old pos as parent

		Json::Value* typeCursor = m_typeSection.cursor;    // Store the current pos
		m_typeSection.cursor = &(*typeCursor)[section];    // Set current pos to enter the child
		m_typeSection.cursorParents.push_back(typeCursor); // Store old pos on stack.
		return result();
	}
	// if section is nullptr, climb up to the parent.
	else if(section == nullptr)
	{
		if(!m_dataSection.cursorParents.empty() && !m_typeSection.cursorParents.empty())
		{
			m_dataSection.cursor = m_dataSection.cursorParents.back();
			m_dataSection.cursorParents.pop_back();

			m_typeSection.cursor = m_typeSection.cursorParents.back();
			m_typeSection.cursorParents.pop_back();
			return result();
		}
		return error(ERROR, "failed to set cursor at " + String(section));
	}
	return error(SUBSECTION_NOT_FOUND, "subsection '"+String(section)+"' was not found prior to calling");
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