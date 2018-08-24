///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.cpp
//
//  A reference to a resource. Contains an instance of File.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceReference.h"
#include "IDocument.h"

OPEN_NAMESPACE(Elf);

ELF_MIRROR_REGISTRATION
{
	ELF_MIRROR_DEFINE_NAMED(IDocument, "Elf::IDocument");
}

OPEN_NAMESPACE(Document);

SharedPtr<IDocument> MakeBlank(const String& type)
{
	Mirror::Type docType = Mirror::Type::get_by_name(type);
	if (docType.is_valid())
	{
		auto methods = docType.get_methods();
		for (auto method : methods)
		{
			if (!method.is_valid())  continue;
			if (!method.is_static()) continue;

			if (method.get_metadata(IDocument::MAKER_FUNCTION_BLANK) == true)
			{
				// found the maker function. call it.
				Mirror::Variant document = method.invoke({});
				if (document.is_valid() && document.can_convert<SharedPtr<IDocument>>())
				{
					return document.convert<SharedPtr<IDocument>>();
				}
			}
		}
	}
	return SharedPtr<IDocument>(nullptr);
}

SharedPtr<IDocument> MakeFromData(const String& type, const Vector<char>& data)
{
	Mirror::Type docType = Mirror::Type::get_by_name(type);
	if (docType.is_valid())
	{
		auto methods = docType.get_methods();
		for (auto method : methods)
		{
			if (!method.is_valid())  continue;
			if (!method.is_static()) continue;

			if (method.get_metadata(IDocument::MAKER_FUNCTION_DATA) == true)
			{
				Mirror::Variant document = method.invoke({}, data);
				if (document.is_valid() && document.can_convert<SharedPtr<IDocument>>())
				{
					return document.convert<SharedPtr<IDocument>>();
				}
			}
		}
	}
	return SharedPtr<IDocument>(nullptr);
}

CLOSE_NAMESPACE(Document);

ResourceReference::ResourceReference()
{
}

ResourceReference::~ResourceReference()
{
}

CLOSE_NAMESPACE(Elf);