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
	using Task_t = std::function<ResourceLoader::LoadResult(const ResourceReference&)>;
	using Future_t = std::future<ResourceLoader::LoadResult>;
	using Promise_t = std::promise<ResourceLoader::LoadResult>;

	struct LoadOp final
	{
		LoadOp(ResourceLoader* loader, ResourceReference* ref);
		~LoadOp();

		ResourceLoader*                    loader;
		ResourceReference*                 ref;
#ifndef FIRE_FINAL
		String             filename;
#endif
	};
public:
	ResourceMgr();
	~ResourceMgr();

	template <class ResourceType_t>
	void Load(ResourceReference& ref)
	{
		ResourceLoader* loader = GetLoader(ResourceType_t::MyResourceType());
		FIRE_ASSERT_MSG(loader, "no loader installed for this resource type");
		Load(std::move(LoadOp(loader, &ref)));
	}
	void Load(LoadOp&& loadOp);

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

	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<LoadOp> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	Vector<std::pair<const ResourceTypeID*, ResourceLoader*>> _loaders;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif