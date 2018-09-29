///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceHandle
//
//  A weak handle to a resource object.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEHANDLE_H_
#define LIBIO_RESOURCEHANDLE_H_
#pragma once

#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>
#include "ResourceLoader.h"
#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

struct ResourceHandleErrors
{
	FIRE_ERRORCODE(NULL_RESOURCE);
};

/**
	Defines a reference counted smart pointer to an IResourceObject. The load status of the handle
	can be checked using the provided functions.
 **/
class ResourceHandle final
{
	friend class ResourceCache;
	friend class ResourceMgr;
public:
	// make an empty handle that's waiting for a resource.
	ResourceHandle();
	ResourceHandle(std::future<ResourceLoader::LoadResult>&& future);

	// pass a resource pointer to the object.
	ResourceHandle(const RefPtr<IResourceObject>& obj);
	ResourceHandle(ResourceHandle&& other);

	~ResourceHandle();

	// Meant to be used with a RefPtr.
	ResourceHandle(const ResourceHandle& other) = delete;
	ResourceHandle& operator=(const ResourceHandle& handle) = delete;
	ResourceHandle& operator=(ResourceHandle&& handle) = delete;

	template<class T>
	const RefPtr<T> Get() const
	{
		return std::dynamic_pointer_cast<T>(_obj);
	}

	template<class T>
	RefPtr<T> Get()
	{
		return std::dynamic_pointer_cast<T>(_obj);
	}

	/**
		Retrieve the error.
	 **/
	Error GetError() const;

	/**
		Retrieve whether or not this ResourceHandle has been passed through the loader and has some kind of state.
	 **/
	bool IsFinished() const;

	/**
		Retrieve whether or not the ResourceHandle is reporting an error.
	 **/
	bool HasError() const;

	/**
		Release the resource from this handle as well as the active error.
	 **/
	void Release();

private:
	void SetFuture(std::future<ResourceLoader::LoadResult>&& future);

	void PullFutureData() const; // this physically hurts me...

	ResourcePtr DoGet() const;

#ifndef FIRE_FINAL
	void SetFilename(const String& filename);
#endif

	mutable Error                                   _error;
	mutable std::future<ResourceLoader::LoadResult> _future;
	mutable ResourcePtr                             _obj;
	mutable bool                                    _futurePulled{ false };

#ifndef FIRE_FINAL
	String _filename;
#endif
};

CLOSE_NAMESPACE(Firestorm);

#endif