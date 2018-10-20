///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Resource
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
#include "ResourceLoader.h"
#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

//struct ResourceHandleErrors
//{
//	FIRE_ERRORCODE(NULL_RESOURCE);
//};
//
///**
//	Defines a handle to a resource. A valid instance of this is only retrievable by a call to
//	#ResourceMgr::Load. Instances can be stored in client classes, however this is done with
//	the understanding that they will eventually be assigned to an instance
// **/
//class Resource 
//{
//	friend class ResourceCache;
//	friend class ResourceMgr;
//public:
//	// make an empty handle that's waiting for a resource.
//	Resource();
//	Resource(future<ResourceLoader::LoadResult>&& future);
//
//	// move only
//	Resource(Resource&& other);
//	Resource(const Resource& other);
//
//	virtual ~Resource();
//
//	Resource& operator=(Resource&& handle);
//	Resource& operator=(const Resource& handle);
//
//	template<class T>
//	const RefPtr<T> Get() const
//	{
//		return eastl::dynamic_pointer_cast<T>(PullData());
//	}
//
//	template<class T>
//	RefPtr<T> Get()
//	{
//		return eastl::dynamic_pointer_cast<T>(PullData());
//	}
//
//	/**
//		Check whether or not this instance of the Resource is valid.
//		\note A Resource is valid when the following conditions are met.
//		- A Future object was passed to the instance either through an assignment or construction (or both).
//	 **/
//	bool IsValid() const;
//
//	/**
//		Retrieve whether or not this Resource has been passed through the loader and has some kind of state.
//	 **/
//	bool IsFinished() const;
//
//	/**
//		Retrieve whether or not the Resource is reporting an error.
//	 **/
//	bool HasError() const;
//
//	/**
//		Retrieve the error.
//	**/
//	Error GetError() const;
//
//	/**
//		Release the resource from this handle as well as the active error.
//	 **/
//	void Release();
//
//private:
//	ResourcePtr PullData() const;
//
//	mutable Error                              _error;
//	mutable future<ResourceLoader::LoadResult> _future;
//	mutable ResourcePtr                        _obj;
//	mutable bool                               _futurePulled{ false };
//	mutable bool                               _isFinished{ false };
//
//	bool _hasFuture{ false };
//	bool _hasError{ false };
//};

CLOSE_NAMESPACE(Firestorm);

#endif