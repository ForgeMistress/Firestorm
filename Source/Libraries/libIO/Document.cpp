///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Document
//
//  A document.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Document.h"
#include "IDocument.h"

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(Document);

Mirror::Method GetMakerFunction(Mirror::Type docType, IDocument::Metadata meta)
{
	auto methods = docType.get_methods();
	for(auto method : methods)
	{
		if(!method.is_valid())  continue;
		if(!method.is_static()) continue;

		if(method.get_metadata(meta) == true)
		{
			return method;
		}
	}
	return rttr::detail::create_invalid_item<rttr::method>();
}

RefPtr<IDocument> MakeBlank(const String& type)
{
	String finalName("Firestorm::Document::" + type);
	Mirror::Type docType = Mirror::Type::get_by_name(finalName);
	if(docType.is_valid())
	{
		auto method = GetMakerFunction(docType, IDocument::MAKER_FUNCTION_BLANK);
		if(method.is_valid())
		{
			Mirror::Variant document = method.invoke({});
			if(document.is_valid())
				if(document.can_convert<RefPtr<IDocument>>())
					return document.convert<RefPtr<IDocument>>();
		}
	}
	return RefPtr<IDocument>(nullptr);
}

RefPtr<IDocument> MakeFromData(const String& type, const Vector<char>& data)
{
	String finalName("Firestorm::Document::" + type);
	Mirror::Type docType = Mirror::Type::get_by_name(finalName);
	if(docType.is_valid())
	{
		auto method = GetMakerFunction(docType, IDocument::MAKER_FUNCTION_DATA);
		if(method.is_valid())
		{
			Mirror::Variant document = method.invoke({}, data);
			if(document.is_valid())
				if(document.can_convert<RefPtr<IDocument>>())
					return document.convert<RefPtr<IDocument>>();
		}
	}
	return RefPtr<IDocument>(nullptr);
}

CLOSE_NAMESPACE(Document);

CLOSE_NAMESPACE(Firestorm);