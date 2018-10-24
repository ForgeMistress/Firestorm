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

#include <future>

#include "ResourceHandle.h"
#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

struct IResourceCache
{
protected:
	friend class ResourceMgr;

	virtual ~IResourceCache() {}

	virtual bool IsCached(const char* filename) = 0;

	virtual void CacheResourceInstance(const char* filename, eastl::shared_ptr<IResourceObject> resourcePtr) = 0;
		//class Application& app, const char* filename) = 0;

	virtual void ClearUnusedResources() = 0;
};

template<class Res>
struct ResourceCache : public IResourceCache
{
	virtual ~ResourceCache() {}

	Resource<Res> GetCached(const char* filename)
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		auto& cacheEntry = _cache[filename];
		eastl::shared_ptr<Res> ptr(eastl::dynamic_pointer_cast<Res>(cacheEntry._ptr));
		return Resource<Res>(ptr, filename);
	}

	virtual void CacheResourceInstance(const char* filename, eastl::shared_ptr<IResourceObject> resourcePtr)//class Application& app, const char* filename, Args&&... args) override
	{
		using shared_future = std::shared_future<LoadResult>;

		std::unique_lock<std::mutex> lock(_cacheLock);
		eastl::shared_ptr<Res> ptr(nullptr);
		if(_cache.find(filename) == _cache.end())
		{
			//shared_future fut(eastl::forward<std::future<LoadResult>>(future));
			//ptr = eastl::make_shared<Res>(eastl::forward<Application>(app), eastl::forward<Args>(args)...);

			_cache[filename] = ResourceCacheEntry{ resourcePtr };
		}
	}

	virtual bool IsCached(const char* filename) override
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		return _cache.find(string(filename)) != _cache.end();
	}

	virtual void ClearUnusedResources() override
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		for(auto cacheItr : _cache)
		{
			if(cacheItr.second._ptr.use_count() == 1)
			{
				// erase one and return
				_cache.erase(cacheItr.first);
				return;
			}
		}
	}

	std::mutex _cacheLock;

	struct ResourceCacheEntry
	{
		eastl::shared_ptr<IResourceObject> _ptr;
	};
	eastl::unordered_map<eastl::string, ResourceCacheEntry> _cache;
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