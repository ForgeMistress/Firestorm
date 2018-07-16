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
}

JSONDocumentReader::~JSONDocumentReader()
{
}

Result<char, Error> JSONDocumentReader::ReadChar(const char* key) const
{
	return Result<char, Error>(0);
}

Result<uchar, Error> JSONDocumentReader::ReadUChar(const char* key) const
{
	return Result<uchar, Error>(0);
}

Result<int8_t, Error> JSONDocumentReader::ReadInt8(const char* key) const
{
	return Result<int8_t, Error>(0);
}

Result<uint8_t, Error> JSONDocumentReader::ReadUInt8(const char* key) const
{
	return Result<uint8_t, Error>(0);
}

Result<int16_t, Error> JSONDocumentReader::ReadInt16(const char* key) const
{
	return Result<int16_t, Error>(0);
}

Result<uint16_t, Error> JSONDocumentReader::ReadUInt16(const char* key) const
{
	return Result<uint16_t, Error>(0);
}

Result<int32_t, Error> JSONDocumentReader::ReadInt32(const char* key) const
{
	return Result<int32_t, Error>(0);
}

Result<uint32_t, Error> JSONDocumentReader::ReadUInt32(const char* key) const
{
	return Result<uint32_t, Error>(0);
}

Result<float, Error> JSONDocumentReader::ReadFloat(const char* key) const
{
	return Result<float, Error>(0.0f);
}

Result<double, Error> JSONDocumentReader::ReadDouble(const char* key) const
{
	return Result<double, Error>(0.0);
}
Result<String, Error> JSONDocumentReader::ReadString(const char* key) const
{
	return Result<String, Error>(String(""));
}

Result<Mirror::Type, Error> JSONDocumentReader::GetType(const char* key) const
{
	// Check the type section.
	/*if(m_typeSection.m_cursor == nullptr)
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

	}*/

	return Result<Mirror::Type, Error>(rttr::detail::get_invalid_type());
}

Result<IDocumentReader*, Error> JSONDocumentReader::FindSubsection(const char* sectionName)
{
	/*assert(m_dataSection.cursor != nullptr && m_typeSection.cursor != nullptr);
	if(m_dataSection.cursor->isMember(sectionName) && m_typeSection.cursor->isMember(sectionName))
	{
		m_foundSubsection = sectionName;
		return result(this);
	}
	std::stringstream ss;
	ss << "subsection " << sectionName << " was not found.";*/
	return Result<IDocumentReader*, Error>(this); //error(FIND_SUBSECTION_FAILED, "");
}

Result<IDocumentReader*, Error> JSONDocumentReader::EnterSubsection(const char* sectionName)
{
	Result<IDocumentReader*, Error> findss = FindSubsection(sectionName);
	if (findss.has_value())
	{
		auto setcursor = SetCursor(sectionName);
		if(setcursor.has_value())
		{
			return Result<IDocumentReader*, Error>(this);
		}
		return Result<IDocumentReader*, Error>(this);//error(ENTER_SUBSECTION_FAILED, "failed to enter subsection " + String(sectionName));
	}
	return findss;
}

Result<IDocumentReader*, Error> JSONDocumentReader::LeaveSubsection()
{
	return Result<IDocumentReader*, Error>(this);
}

Result<IDocumentReader*, Error> JSONDocumentReader::ReadDocument(SharedPtr<IDocument>& document)
{
	return Result<IDocumentReader*, Error>(this);
}

Result<void, Error> JSONDocumentReader::Section::SetPosFromRoot(const std::initializer_list<String>& list)
{
	if(list.size() > 0)
	{
		MoveToChild();
		for(const String& item : list)
		{
			Result<void, Error> moveResult = MoveToChild(item);
			if(!moveResult.has_value())
			{
				return moveResult;
			}
		}
	}
	return Result<void, Error>();
}

Result<void, Error> JSONDocumentReader::Section::MoveToChild(const String& toChild)
{
	assert(m_cursor != nullptr);
	if(toChild == "")
	{
		// Set it to the root of the document and clear the parent stack.
		m_cursor = &m_data;
		m_cursorParents.clear();
		return Result<void, Error>();
	}
	if(m_cursor->isMember(toChild))
	{
		m_cursor = &(*m_cursor)[toChild];
	}
	// TODO: Re-evaluate this.
	return Result<void, Error>();
}

Result<void, Error> JSONDocumentReader::Section::MoveToParent()
{
	return Result<void, Error>();
}

Result<void, Error> JSONDocumentReader::Section::MoveToSibling(const String& toSibling)
{
	return Result<void, Error>();//void_result();
}

#define ELF_FAILRETURN(RTYPE, OP) { \
	RTYPE rc = OP ; \
	if(!rc.has_value()) {\
		return rc; \
	} \
}

Result<void, Error> JSONDocumentReader::SetCursor(const char* section)
{
	if(m_foundSubsection == section)
	{
		/*Section& typeSection = m_typeSection;
		String sec(section);
		m_dataSection.MoveToChild(section)
			.map_error([](const Error& err) {
				
			})
			.and_then([&typeSection, &sec](void) {
				typeSection.MoveToChild(sec);
			});*/
		//m_typeSection.MoveToChild(section)
		/*Json::Value* dataCursor = m_dataSection.cursor;    // Store the current pos
		m_dataSection.cursor = &(*dataCursor)[section];    // Set current pos to enter the child
		m_dataSection.cursorParents.push_back(dataCursor); // Store old pos as parent

		Json::Value* typeCursor = m_typeSection.cursor;    // Store the current pos
		m_typeSection.cursor = &(*typeCursor)[section];    // Set current pos to enter the child
		m_typeSection.cursorParents.push_back(typeCursor); // Store old pos on stack.
		return result();*/
	}
	// if section is nullptr, climb up to the parent.
	else if(section == nullptr)
	{
		/*if(!m_dataSection.cursorParents.empty() && !m_typeSection.cursorParents.empty())
		{
			m_dataSection.cursor = m_dataSection.cursorParents.back();
			m_dataSection.cursorParents.pop_back();

			m_typeSection.cursor = m_typeSection.cursorParents.back();
			m_typeSection.cursorParents.pop_back();
			return result();
		}*/
		return ELF_ERROR(ERROR, "failed to set cursor at " + String(section));
	}
	return ELF_ERROR(SUBSECTION_NOT_FOUND, "subsection '" + String(section) + "' was not found prior to calling");
	//tl::make_unexpected<Error>(Error(SUBSECTION_NOT_FOUND, "subsection '" + String(section) + "' was not found prior to calling"));
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