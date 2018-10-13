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
#ifndef FIRE_FINAL
	const char* Name;
#endif
};

#ifndef FIRE_FINAL
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
#else
#define FIRE_RESOURCE_TYPE( CLASS, LOADER )         \
public:											    \
	friend class ResourceMgr;						\
	friend class LOADER;                            \
	using LoaderType = LOADER;						\
	static const ResourceTypeID* MyResourceType() { \
		static const ResourceTypeID* id = nullptr;	\
		if(id == nullptr)							\
			id = new ResourceTypeID{};				\
		return id;									\
	}                                               \
private:
#endif


/**
	Manages the asynchronous loading of resources.
 **/
class ResourceMgr final
{
private:
	using PromiseT = std::promise<ResourceLoader::LoadResult>;

public:
	ResourceMgr();
	~ResourceMgr();

	/**
		Load up a resource. The load status of said resource can be checked using the
		returned Resource instance.
	 **/
	template <class ResourceType>
	Resource Load(const ResourceReference& ref)
	{
		ResourceLoader* loader = GetLoader(ResourceType::MyResourceType());
		FIRE_ASSERT_MSG(loader, "no loader installed for this resource type");
		return std::move(Load(loader, ref));
	}

	/**
		Install a resource loader to the ResourceMgr. The ResourceType passed into the template
		argument must provide the loader type under an alias (typedef or using declared) called LoaderType.
	 **/
	template<class ResourceType, class... Args_t>
	bool InstallLoader(Args_t&&... args)
	{
		ResourceLoader* loader = new typename ResourceType::LoaderType(std::forward<Args_t>(args)...);
		return InstallLoader(ResourceType::MyResourceType(), loader);
	}

	/**
		Signal to the ResourceMgr that it's time to shut down. This will hang the calling thread until all
		worker threads have been joined.
	 **/
	void Shutdown();

private:
	bool InstallLoader(const ResourceTypeID* resourceType, ResourceLoader* loader);
	ResourceLoader* GetLoader(const ResourceTypeID* type);
	Resource Load(ResourceLoader* loader, const ResourceReference& ref);

	static const char _numThreads{ 4 };

	string _name;
	mutex _queueLock;

	thread _threads[_numThreads];

	queue<function<void(void)>> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	unordered_map<const ResourceTypeID*, UniquePtr<ResourceLoader>> _loaders;

	ResourceCache _cache;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif