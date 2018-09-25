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
#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

class ResourceCache;

/**
	Defines a reference counted smart pointer to an IResourceObject. The load status of the handle
	can be checked using the provided functions.
 **/
class ResourceHandleObject final
{
	friend class ResourceCache;
	friend class ResourceMgr;
public:
	/**
		Defines the states that the ResourceHandleObject can be in at any given time.
	 **/
	enum struct State
	{
		kEmpty,          //< The ResourceHandleObject contains no valid pointer to an IResourceObject.
		kWaitingForLoad, //< The ResourceHandleObject is currently in the load queue and waiting to be loaded.
		kLoading,        //< The ResourceHandleObject is currently being loaded by one of the worker threads.
		kLoaded,         //< The ResourceHandleObject finished loading with no errors.
		kLoadError       //< The ResourceHandleObject finished loading with errors.
	};

	ResourceHandleObject(ResourceCache* cache);
	ResourceHandleObject(std::nullptr_t);
	ResourceHandleObject(ResourceCache* cache, IResourceObject* obj);
	ResourceHandleObject(const ResourceHandleObject& other);
	ResourceHandleObject(ResourceHandleObject&& other);
	
	~ResourceHandleObject();

	ResourceHandleObject& operator=(const ResourceHandleObject& handle);
	ResourceHandleObject& operator=(ResourceHandleObject&& handle);

	bool operator==(const ResourceHandleObject& other);

	template<class T>
	const T* Get()
	{
		return static_cast<const T*>(_obj);
	}

	const IResourceObject* operator->() const;

	template<class T>
	const T* operator->() const
	{
		return static_cast<T*>();
	}

	/**
		Retrieve the present state of this ResourceHandleObject.
		\return state One of #ResourceHandleObject::State
	 **/
	State GetState() const;

	/**
		Release the resource from this handle as well as the active error.
	 **/
	void Release();

private:
	void SetState(State state);

	// Calling this will remove whatever the active ErrorCode is.
	void SetResourcePointer(IResourceObject* obj);

	// Calling this will remove whatever the active object is.
	void SetError(const ErrorCode* error);

	void AddRef();
	void DelRef();

	mutable Mutex _pointerLock;

	const ErrorCode* _error{ nullptr };
	ResourceCache* _cache{ nullptr };
	IResourceObject* _obj{ nullptr };
	std::atomic<State> _state{ State::kEmpty };
};

typedef ResourceHandleObject* ResourceHandle;


class ResourceCache
{
	friend class ResourceMgr;
public:
	ResourceCache();
	~ResourceCache();

	bool HasResource(const String& name);

	bool AddResource(const String& name, IResourceObject* object);

	/**
		Retrieve a resource from the cache and return a handle to that resource.
		If the resource does not exist, then the handle will be an empty and invalid handle.
	 **/
	ResourceHandle GetResource(const String& name) const;

	void ClearOrphanedResources();

private:
	IResourceObject* FindResource(const String& name) const;

	struct CacheEntry
	{
		String           name;
		IResourceObject* object;
	};
	Vector<CacheEntry> _cache;
	mutable ObjectPool<ResourceHandleObject> _handlePool;

	mutable Mutex _lock;
};

CLOSE_NAMESPACE(Firestorm);

#endif