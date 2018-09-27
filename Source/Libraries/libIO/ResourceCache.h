///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceCache
//
//  Cache that contains handles to all of the resources that are presently loaded.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCECACHE_H_
#define LIBIO_RESOURCECACHE_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Result.h>
#include "IResourceObject.h"


OPEN_NAMESPACE(Firestorm);

class ResourceCache;

/**
	Defines the states that the ResourceHandle can be in at any given time.
**/
enum struct ResourceHandleState
{
	kEmpty,          //< The ResourceHandle contains no valid pointer to an IResourceObject.
	kWaitingForLoad, //< The ResourceHandle is currently in the load queue and waiting to be loaded.
	kLoading,        //< The ResourceHandle is currently being loaded by one of the worker threads.
	kLoaded,         //< The ResourceHandle finished loading with no errors.
	kLoadError       //< The ResourceHandle finished loading with errors.
};

/**
	Defines a reference counted smart pointer to an IResourceObject. The load status of the handle
	can be checked using the provided functions.
 **/
class ResourceHandle_ final
{
	friend class ResourceCache;
	friend class ResourceMgr;
public:
	//ResourceHandle_(std::nullptr_t);
	ResourceHandle_(ResourceCache* cache, IResourceObject* obj);
	
	~ResourceHandle_();

	// Meant to be used with a RefPtr.
	ResourceHandle_(const ResourceHandle_& other) = delete;
	ResourceHandle_(ResourceHandle_&& other) = delete;
	ResourceHandle_& operator=(const ResourceHandle_& handle) = delete;
	ResourceHandle_& operator=(ResourceHandle_&& handle) = delete;

	template<class T>
	const T* Get() const
	{
		return static_cast<const T*>(_obj);
	}

	template<class T>
	T* Get()
	{
		return static_cast<T*>(_obj);
	}

	/**
		Retrieve the error.
	 **/
	Error GetError() const;

	/**
		Retrieve whether or not this ResourceHandle holds a valid pointer to a resource object.
	 **/
	bool IsReady() const;

	/**
		Retrieve whether or not the ResourceHandle is reporting an error.
	 **/
	bool HasError() const;

	/**
		Retrieve the present state of this ResourceHandle_.
		\return state One of #ResourceHandle_::State
	 **/
	ResourceHandleState GetState() const;

	/**
		Release the resource from this handle as well as the active error.
	 **/
	void Release();

private:
	void SetState(ResourceHandleState state);

	// Calling this will remove whatever the active ErrorCode is.
	void SetResourcePointer(IResourceObject* obj);

	// Calling this will remove whatever the active object is.
	void SetError(const ErrorCode* error);

#ifndef FIRE_FINAL
	void SetFilename(const String& filename);
#endif

	void AddRef();
	void DelRef();

	mutable Mutex _pointerLock;

	const ErrorCode* _error{ nullptr };
	ResourceCache* _cache{ nullptr };
	IResourceObject* _obj{ nullptr };
	std::atomic<ResourceHandleState> _state{ ResourceHandleState::kEmpty };

#ifndef FIRE_FINAL
	String _filename;
#endif
};

using ResourceHandle = RefPtr<ResourceHandle_>;

class ResourceCache
{
	friend class ResourceMgr;
public:
	ResourceCache();
	~ResourceCache();

	/**
		Retrieve whether or not the resource cache has the particular resource loaded.
	 **/
	bool HasResource(const String& name);

	/**
		Retrieve a resource from the cache and return a handle to that resource.
		If the resource does not exist, then the handle will be an empty and invalid handle.
	 **/
	ResourceHandle GetResource(const String& name) const;

	void ClearOrphanedResources();

private:
	bool AddResource(const String& name, IResourceObject* object);
	IResourceObject* FindResource(const String& name) const;

	struct CacheEntry
	{
		String           name;
		IResourceObject* object;
	};
	Vector<CacheEntry> _cache;
	mutable ObjectPool<ResourceHandle_> _handlePool;

	mutable Mutex _lock;
};

CLOSE_NAMESPACE(Firestorm);

#endif