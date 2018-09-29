///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceMgr
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEMGR_H_
#define LIBIO_RESOURCEMGR_H_
#pragma once

#include "ResourceLoader.h"
#include "ResourceCache.h"
#include "ResourceReference.h"
#include "ResourceHandle.h"
#include "IResourceObject.h"

#include <libCore/Result.h>
#include <libCore/Expected.h>
#include <libCore/ObjectPool.h>

#include <libMirror/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

class ResourceLoader;
class ResourceCache;
class ResourceReference;

struct ResourceTypeID
{
	const char* Name;
};

#define FIRE_RESOURCE_TYPE( CLASS, LOADER )         \
public:											    \
	friend class ResourceMgr;						\
	friend class LOADER;                            \
	using LoaderType = LOADER;						\
	static const ResourceTypeID* MyResourceType() { \
		static const ResourceTypeID* id = nullptr;	\
		if(id == nullptr)							\
			id = new ResourceTypeID{				\
				#CLASS								\
			};										\
													\
		return id;									\
	}                                               \
private:


/**
	\class ResourceMgr

	Manages the asynchronous loading of resources.
 **/
class ResourceMgr final
{
private:
	using PromiseT = std::promise<ResourceLoader::LoadResult>;
	struct LoadOp final
	{
		LoadOp(ResourceLoader* loader, ResourceReference* ref, PromiseT* promise);
		LoadOp(LoadOp&& other);
		~LoadOp();

		ResourceLoader*     loader;
		ResourceReference*  ref;
		PromiseT*           promise;
#ifndef FIRE_FINAL
		String filename;
#endif
	};

	Future<ResourceLoader::LoadResult> Load(ResourceLoader* loader, ResourceReference* ref);

public:
	ResourceMgr();
	~ResourceMgr();

	/**
		Load up a resource. The load status of said resource can be checked using the
		returned ResourceHandle instance.
	 **/
	template <class ResourceType_t>
	Future<ResourceLoader::LoadResult> Load(ResourceReference& ref)
	{
		ResourceLoader* loader = GetLoader(ResourceType_t::MyResourceType());
		FIRE_ASSERT_MSG(loader, "no loader installed for this resource type");
		return std::move(Load(loader, &ref));
	}

	template<class ResourceType_t, class... Args_t>
	bool InstallLoader(Args_t&&... args)
	{
		ResourceLoader* loader = new typename ResourceType_t::LoaderType(std::forward<Args_t>(args)...);
		return InstallLoader(ResourceType_t::MyResourceType(), loader);
	}

	void Shutdown();

private:
	bool InstallLoader(const ResourceTypeID*, ResourceLoader* loader);
	ResourceLoader* GetLoader(const ResourceTypeID* type);

	// retrieve either a resource handle to an existing resource or an empty handle if the resource
	// is not loaded.
	RefPtr<ResourceHandle> GetResourceHandle(const String& name) const;

	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<UniquePtr<LoadOp>> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	UnorderedMap<const ResourceTypeID*, UniquePtr<ResourceLoader>> _loaders;

	mutable
	ObjectPool<ResourceHandle> _handlePool;

	ResourceCache _cache;

	// void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif