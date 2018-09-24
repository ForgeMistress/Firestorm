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

struct ResourcePtr
{
public:
	ResourcePtr(ResourceCache& cache, const String& name, IResourceObject* obj);
	ResourcePtr(const ResourcePtr& other);

	~ResourcePtr();

	const String& GetName() const;

private:
	ResourceCache&   _cache;
	String           _name;
	IResourceObject* _obj{nullptr};
};

class ResourceCache
{
public:
	ResourceCache();
	~ResourceCache();

	void AddResource(const String& name, IResourceObject* resourceObject);
	ResourcePtr GetResource(const String& name) const;

private:
	Vector<IResourceObject*> _cache;
};

CLOSE_NAMESPACE(Firestorm);

#endif