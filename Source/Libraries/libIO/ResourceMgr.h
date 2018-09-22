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

#include <libCore/Result.h>
#include <libCore/Expected.h>
#include <libCore/ObjectPool.h>

#include <libMirror/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

class IResourceObject;
class ResourceLoader;
class ResourceCache;
class ResourceReference;

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

		ResourceLoader*    loader;
		ResourceReference* ref;
		Promise_t*         promise;
	};
	ObjectPool<LoadOp> _opPool;
public:
	ResourceMgr();
	~ResourceMgr();

	template <class ResourceType_t>
	void Load(ResourceReference& ref)
	{
		ResourceLoader* loader = GetLoader(ResourceType_t::MyType());
		FIRE_ASSERT_MSG(loader, "no loader installed for this resource type");
		Load(_opPool.Get(loader, &ref));
	}
	void Load(LoadOp* loadOp);

	template<class Res_t>
	bool InstallLoader(ResourceLoader* loader)
	{
		return InstallLoader(Res_t::MyType(), loader);
	}

	void Shutdown();

private:
	bool InstallLoader(Mirror::Type type, ResourceLoader* loader);
	ResourceLoader* GetLoader(Mirror::Type type);

	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<LoadOp*> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	Vector<std::pair<Mirror::Type, ResourceLoader*>> _loaders;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif