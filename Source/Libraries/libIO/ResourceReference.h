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
		
	 **/


private:
	friend class FileIOMgr;

	void SetResult(const Result<void, Error>& result);

	Mutex _mutex;
	Result<void, Error> _lastLoadResult;

	String _resourcePath;
	bool _isLoaded{ false };
};

CLOSE_NAMESPACE(Firestorm);

#endif