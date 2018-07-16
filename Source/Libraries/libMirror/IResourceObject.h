////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//<
//<  \file ResourceObject.h
//<
//<  Contains \ref IResourceObject
//<
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_RESOURCEOBJECT_H_
#define LIBMIRROR_RESOURCEOBJECT_H_
#pragma once

#include "Object.h"
#include <libIO/IDocumentReader.h>
#include <libIO/IDocumentWriter.h>

OPEN_NAMESPACE(Elf);

class IResourceObject
{
	ELF_MIRROR_DECLARE(IResourceObject);
public:
	virtual ~IResourceObject() {}

	virtual Result<void, Error> Load(const SharedPtr<IDocumentReader>& reader) = 0;
	virtual Result<void, Error> Save(SharedPtr<IDocumentWriter>& writer) const = 0;
};

CLOSE_NAMESPACE(Elf);
#endif