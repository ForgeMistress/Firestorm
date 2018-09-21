///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.h
//
//  Contains \ref ResourceReference.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEREFERENCE_H_
#define LIBIO_RESOURCEREFERENCE_H_
#pragma once

#include <future>
#include "IResourceObject.h"
#include "ResourceMgr.h"

OPEN_NAMESPACE(Firestorm);

/**
	\class ResourceReference

	This class instance does not contain any data. Instead, it merely defines a reference to a resource.
 **/
class ResourceReference : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(ResourceReference, Mirror::Object);
public:
	ResourceReference();
	virtual ~ResourceReference();

	/**
		Retrieve the path to the resource on disk.
	 **/
	const String& GetResourcePath() const;

	/**
		Retrieve whether or not the file is loaded.
	 **/
	bool GetIsLoaded() const;

	/**
		Check whether or not the resource loaded with an error.
	 **/
	bool HasError();

	/**
		Check whether or not the ResourceReference is holding a resource. Opposite of HasError
	 **/
	bool HasResource();

	/**
		Retrieve the resource if it is finished loading.
	 **/
	RefPtr<IResourceObject> GetResource();

private:
	friend class ResourceMgr;
	void SetResourcePath(const String& path);

	void SetFuture(std::future<ResourceMgr::LoadResult>&& future);

	std::future<ResourceMgr::LoadResult> _future;

	String _resourcePath;
	bool _isLoaded{ false };
};

CLOSE_NAMESPACE(Firestorm);

#endif