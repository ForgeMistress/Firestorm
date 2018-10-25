///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceMgr
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEMGR_H_
#define LIBIO_RESOURCEMGR_H_
#pragma once

#include "ResourceLoader.h"
#include "ResourceReference.h"
#include "ResourceHandle.h"
#include "IResourceObject.h"

#include <libCore/Result.h>
#include <libCore/Expected.h>
#include <libCore/TaskGraph.h>
#include <libCore/Logger.h>
#include <taskflow/taskflow.hpp>

#include <libCore/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

class ResourceLoader;

struct ResourceTypeID
{
#ifndef FIRE_FINAL
	const char* Name;
#endif
};

#define FIRE_RESOURCE_DECLARE(RES)                                                    \
private:                                                                              \
	friend class ResourceMgr;														  \
	friend struct ResMgrProxy;                                                        \
	using PtrType = eastl::shared_ptr<RES>;                                           \
	FIRE_MIRROR_DECLARE(RES);														  \
	struct LoadOp																	  \
	{																				  \
		ResourceReference ResourceRef;												  \
		class Application& App;														  \
		class ResourceMgr& Mgr;                                                       \
																					  \
		LoadOp(class Application& app, class ResourceMgr& mgr, const char* filename); \
		LoadResult operator()();					                                  \
		LoadResult DoOperation(PtrType& Resource);                                    \
	};                                                                                \
	static ResourceCache<RES> _s_cache;                                               \
	virtual IResourceCache* GetCache() const { return &_s_cache; }                    \
private:
	

#define FIRE_RESOURCE_DEFINE(RES)                                                                  \
ResourceCache<RES> RES::_s_cache;                                                                  \
RES::LoadOp::LoadOp(Application& app, ResourceMgr& mgr, const char* filename)	                   \
: ResourceRef(filename)															                   \
, App(app)																		                   \
, Mgr(mgr)                                                                                         \
{																				                   \
}																				                   \
LoadResult RES::LoadOp::operator()()								                               \
{																						           \
	eastl::string path(ResourceRef.GetPath());                                                     \
	return DoOperation(_s_cache.GetCached(path.c_str()).GetShared());                              \
}																						           \
LoadResult RES::LoadOp::DoOperation(PtrType& Resource)

class ResourceMgr final
{
public:
	ResourceMgr(class Application& app);

	/**
		Schedule a resource of the provided type and the supplied filename for load and return a Resource
		handle that can be used to track the progress of that load. For further details on tracking loads, see
		the Resource type itself.
	 **/
	template<class ResType, class ReturnType = ResType, class... Args>
	Resource<ReturnType> Load(const char* filename, Args&&... args)
	{
		AddResourceCache(&ResType::_s_cache);
		if(ResType::_s_cache.IsCached(filename))
		{
			FIRE_LOG_DEBUG("/!\\ RETURNING CACHED '%s' /!\\", filename);
			return Resource<ReturnType>(ResType::_s_cache.GetCached(filename));
		}

		ResType::_s_cache.CacheResourceInstance(
			filename,
			eastl::make_shared<ResType>(
				_app,
				eastl::forward<Args>(args)...
			)
		);

		typename ResType::LoadOp loadOp(_app, *this, filename);
		LoadResult result(loadOp());
		
		return Resource<ReturnType>(ResType::_s_cache.GetCached(filename));
	}

	void AddResourceCache(IResourceCache* cache)
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		for(size_t i=0; i<_caches.size(); ++i)
		{
			if(_caches[i] == cache)
			{
				return;
			}
		}
		_caches.push_back(cache);
	}

	void CleanOldResources()
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		for(size_t i=0; i<_caches.size(); ++i)
		{
			_caches[i]->ClearUnusedResources();
		}
	}

public:
	class Application& _app;
	class TaskGraph&   _tg;
	std::mutex _cacheLock;
	mutable vector<IResourceCache*> _caches;
};

struct ResMgrProxy
{
	Application& _app;
	ResourceMgr& _mgr;
	TaskGraph& _tf;
	tf::SubflowBuilder& _builder;
	string _thisFilename;

	ResMgrProxy(Application& app, ResourceMgr& mgr, tf::SubflowBuilder& builder, const char* thisFilename);

	template<class ResType>
	Resource<ResType> LoadDependency(const char* filename)
	{
		if(ResType::_s_cache.IsCached(filename))
		{
			FIRE_LOG_DEBUG("/!\\ RETURNING CACHED '%s' /!\\", filename);
			// need to figure out how to get thhese resource handles to reference the same pointer
			return ResType::_s_cache.GetCached(filename);
		}
	
		FIRE_ASSERT(_tf.has(_thisFilename.c_str()));
		auto[task, fut] = _tf.emplace(
			[&, this, fname = string(filename)] (tf::SubflowBuilder& builder) -> LoadResult {
				ResType::LoadOp loadOp(_app, *this, fname.c_str());
				return loadOp(builder);
			}, filename);
		ResType::_s_cache.CacheResourceInstance(_app, filename, eastl::move(fut));
		return ResType::_s_cache.GetCached(filename);
	}
};

//**
//	Manages the asynchronous loading of resources.
// **/
//class ResourceMgr final
//{
//private:
//	using PromiseT = std::promise<ResourceLoader::LoadResult>;
//
//public:
//	ResourceMgr();
//	~ResourceMgr();
//
//	/**
//		Load up a resource. The load status of said resource can be checked using the
//		returned Resource instance.
//	 **/
//	template <class ResourceType>
//	Resource Load(const ResourceReference& ref)
//	{
//		ResourceLoader* loader = GetLoader(ResourceType::MyResourceType());
//		FIRE_ASSERT_MSG(loader, "no loader installed for this resource type");
//		return std::move(Load(loader, ref));
//	}
//
//	/**
//		Install a resource loader to the ResourceMgr. The ResourceType passed into the template
//		argument must provide the loader type under an alias (typedef or using declared) called LoaderType.
//	 **/
//	template<class ResourceType, class... Args_t>
//	bool InstallLoader(Args_t&&... args)
//	{
//		ResourceLoader* loader = new typename ResourceType::LoaderType(std::forward<Args_t>(args)...);
//		return InstallLoader(ResourceType::MyResourceType(), loader);
//	}
//
//	/**
//		Signal to the ResourceMgr that it's time to shut down. This will hang the calling thread until all
//		worker threads have been joined.
//	 **/
//	void Shutdown();
//
//private:
//	bool InstallLoader(const ResourceTypeID* resourceType, ResourceLoader* loader);
//	ResourceLoader* GetLoader(const ResourceTypeID* type);
//	Resource Load(ResourceLoader* loader, const ResourceReference& ref);
//
//	static const char _numThreads{ 4 };
//
//	string _name;
//	mutex _queueLock;
//
//	thread _threads[_numThreads];
//
//	queue<function<void(void)>> _queue;
//	std::condition_variable _cv;
//	bool _quit{ false };
//
//	unordered_map<const ResourceTypeID*, UniquePtr<ResourceLoader>> _loaders;
//
//	ResourceCache _cache;
//
//	void ThreadRun();
//};

CLOSE_NAMESPACE(Firestorm);
#endif