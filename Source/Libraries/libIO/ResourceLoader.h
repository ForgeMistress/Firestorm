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

#define FIRE_LOAD_SUCCESS( PTR ) \
	::Firestorm::ResourceLoader::LoadResult( PTR )

#define FIRE_LOAD_FAIL( ERROR_CODE, DETAILS ) \
	::Firestorm::ResourceLoader::LoadResult( ::Firestorm::Error(ERROR_CODE, DETAILS) )

OPEN_NAMESPACE(Firestorm);

class ResourceReference;
class ResourceMgr;

class ResourceLoader
{
public:
	using PtrHandler = std::function<void(IResourceObject*)>;
	struct LoadResult
	{
	public:
		LoadResult();
		LoadResult(ResourcePtr&& resource);
		LoadResult(const Error& error);

		template<class T>
		RefPtr<T> GetResource() const
		{
			return std::dynamic_pointer_cast<T>(_resource);
		}

		Error GetError() const;

		bool HasError() const;

	private:
		ResourcePtr  _resource;
		Error        _error;
	};

	ResourceLoader();
	virtual ~ResourceLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref);
};

CLOSE_NAMESPACE(Firestorm);

#endif
