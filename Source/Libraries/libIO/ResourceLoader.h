///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceLoader
//
//  The base class for a resource loader.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCELOADER_H_
#define LIBIO_RESOURCELOADER_H_
#pragma once

#include "IResourceObject.h"
#include <libCore/Result.h>

OPEN_NAMESPACE(Firestorm);

class ResourceReference;
class ResourceMgr;

class ResourceLoader
{
public:
	using PtrHandler = std::function<void(IResourceObject*)>;
	using LoadResult = Result<std::pair<IResourceObject*, PtrHandler>, Error>;

	ResourceLoader();
	virtual ~ResourceLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref);
};

CLOSE_NAMESPACE(Firestorm);

#endif
