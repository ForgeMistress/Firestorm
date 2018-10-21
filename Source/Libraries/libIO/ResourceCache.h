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

struct IResourceCache
{
protected:
	virtual ~IResourceCache() {}
	template<class Res>
	eastl::shared_ptr<IResourceObject> MakeResource(class Application& app, const char* filename)
	{
		eastl::shared_ptr<IResourceObject> resource(eastl::make_shared<Res>(app));
		_baseCache[filename] = resource;
		return resource;
	}

	eastl::unordered_map<eastl::string, eastl::weak_ptr<IResourceObject>> _baseCache;
};

template<class Res>
struct ResourceCache
{
	bool IsCached(const char* filename)
	{
		std::unique_lock<std::mutex> lock(_cacheLock);                                    
		return _cache.find(string(filename)) != _cache.end();
	}

	Resource<Res> CacheResourceInstance(class Application& app, const char* filename, std::future<LoadResult>&& future)
	{
		Resource<Res> resource(eastl::make_shared<Res>(app), eastl::forward<std::future<LoadResult>>(future), filename);
		std::unique_lock<std::mutex> lock(_cacheLock);
		//_cacheLock.lock();
		//_cache[filename] = eastl::forward<Resource<Res>>(resource);

		_cacheLock.unlock();
		return _cache[filename];
	}

	Resource<Res> GetCached(const char* filename)
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		return _cache[string(filename)];
	}
	eastl::unordered_map<eastl::string, Resource<Res>> _cache;
	std::mutex _cacheLock;
};

//class ResourceCache final
//{
//public:
//	ResourceCache();
//	~ResourceCache();
//
//	/**
//		Retrieve whether or not the resource cache has the particular resource loaded.
//	 **/
//	bool HasResource(const string& name);
//
//	/**
//		Retrieve a pointer to a loaded resource, or nullptr if the resource does
//		not exist in the cache.
//	 **/
//	ResourcePtr FindResource(const string& name) const;
//
//	/**
//		Clear the cache of any resources that are no longer being referenced
//		by anything.
//	 **/
//	void ClearOrphanedResources();
//
//private:
//	friend class ResourceMgr;
//	bool AddResource(const string& name, const RefPtr<IResourceObject>& object);
//
//	mutable std::mutex _cacheLock;
//	unordered_map<string, RefPtr<IResourceObject>> _cache;
//};

CLOSE_NAMESPACE(Firestorm);

#endif