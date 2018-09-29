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
#include <libCore/RefPtr.h>
#include <libCore/WeakPtr.h>

#include "ResourceHandle.h"
#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

class ResourceCache final
{
public:
	ResourceCache();
	~ResourceCache();

	/**
		Retrieve whether or not the resource cache has the particular resource loaded.
	 **/
	bool HasResource(const String& name);

	ResourcePtr FindResource(const String& name) const;

	void ClearOrphanedResources();

private:
	friend class ResourceMgr;
	bool AddResource(const String& name, const RefPtr<IResourceObject>& object);

	UnorderedMap<String, RefPtr<IResourceObject>> _cache;
	mutable ObjectPool<ResourceHandle>            _handlePool;

	mutable Mutex _lock;
	mutable Mutex _handlePoolLock;
};

CLOSE_NAMESPACE(Firestorm);

#endif