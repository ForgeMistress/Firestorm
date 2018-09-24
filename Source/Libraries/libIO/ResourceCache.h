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

class ResourceHandle final
{
	friend class ResourceCache;
public:
	ResourceHandle();
	ResourceHandle(std::nullptr_t);
	ResourceHandle(const String& name, IResourceObject* obj);
	ResourceHandle(const ResourceHandle& other);
	ResourceHandle(ResourceHandle&& other);
	
	~ResourceHandle();

	ResourceHandle& operator=(const ResourceHandle& handle);
	ResourceHandle& operator=(ResourceHandle&& handle);

	bool operator==(const ResourceHandle& other);

	const String& GetName() const;

	template<class T>
	T* Get()
	{
		return static_cast<T*>(_obj);
	}

private:
	void Set(const String& name, IResourceObject* obj);

	String           _name;
	IResourceObject* _obj{nullptr};
};


class ResourceCache
{
public:
	ResourceCache();
	~ResourceCache();

	bool AddResource(const String& name, IResourceObject* resourceObject, ResourceHandle& outHandle);

	bool HasResource(const String& name);

	ResourceHandle GetResource(const String& name) const;

	void ClearOrphanedResources();

private:
	IResourceObject* FindResource(const String& name) const;

	Mutex _lock;
	struct CacheEntry
	{
		String           name;
		IResourceObject* object;
	};
	Vector<CacheEntry> _cache;
};

CLOSE_NAMESPACE(Firestorm);

#endif