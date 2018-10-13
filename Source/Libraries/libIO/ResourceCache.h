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
	bool HasResource(const string& name);

	/**
		Retrieve a pointer to a loaded resource, or nullptr if the resource does
		not exist in the cache.
	 **/
	ResourcePtr FindResource(const string& name) const;

	/**
		Clear the cache of any resources that are no longer being referenced
		by anything.
	 **/
	void ClearOrphanedResources();

private:
	friend class ResourceMgr;
	bool AddResource(const string& name, const RefPtr<IResourceObject>& object);

	mutable mutex _cacheLock;
	unordered_map<string, RefPtr<IResourceObject>> _cache;
};

CLOSE_NAMESPACE(Firestorm);

#endif