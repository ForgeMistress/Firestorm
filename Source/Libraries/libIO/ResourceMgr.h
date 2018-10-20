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
#include <libCore/TaskGraph.h>
#include <libCore/Logger.h>
#include <taskflow/taskflow.hpp>

#include <libMirror/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

class ResourceLoader;
class ResourceCache;

struct ResourceTypeID
{
#ifndef FIRE_FINAL
	const char* Name;
#endif
};

#define FIRE_RESOURCE_DECLARE( CLASS )                                       \
private:                                                                     \
	using ResultType = Result<ResourcePtr, Error>; \
	using FutureType = std::future<ResultType>; \
	using AsyncType = tuple<tf::Task, FutureType>>;         \
	using FuncType = function<ResultType(const char* filename)>; \
	static FuncType _s_loadFunction;
	//static GenType Generate(ResourceMgr& resourceMgr, const char* filename); \

#define FIRE_RESOURCE_DEFINE( CLASS )

struct ResourceHandleErrors
{
	FIRE_ERRORCODE(NULL_RESOURCE);
};

/**
Defines a handle to a resource. A valid instance of this is only retrievable by a call to
#ResourceMgr::Load. Instances can be stored in client classes, however this is done with
the understanding that they will eventually be assigned to an instance
**/
class Resource_
{
	friend class ResourceCache;
	friend class ResourceMgr;
public:
	// make an empty handle that's waiting for a resource.
	Resource_(eastl::pair<tf::Task, std::future<LoadResult>>&& tuppy);
	// ResourceType(future<LoadResult>&& future);

	// move only
	// ResourceType(ResourceType&& other);
	// ResourceType(const ResourceType& other);

	virtual ~Resource_();

	// ResourceType& operator=(ResourceType&& handle);
	// ResourceType& operator=(const ResourceType& handle);

	template<class T>
	const RefPtr<T> Get() const
	{
		return eastl::dynamic_pointer_cast<T>(PullData());
	}

	template<class T>
	RefPtr<T> Get()
	{
		return eastl::dynamic_pointer_cast<T>(PullData());
	}

	/**
		Retrieve the error.
	**/
	Error GetError() const;

	/**
		Check whether or not this instance of the Resource is valid.
		\note A Resource is valid when the following conditions are met.
		- A Future object was passed to the instance either through an assignment or construction (or both).
	**/
	bool IsValid() const;

	/**
		Retrieve whether or not this Resource has been passed through the loader and has some kind of state.
	**/
	bool IsFinished() const;

	/**
		Retrieve whether or not the Resource is reporting an error.
	**/
	bool HasError() const;

	/**
		Release the resource from this handle as well as the active error.
	**/
	void Release();

private:
	void DoPull() const;
	ResourcePtr PullData() const;

	mutable eastl::pair<tf::Task, std::future<LoadResult>> _tuppy;
	//tf::Task _task;
	//mutable std::future<LoadResult> _future;
	// pulled from future.
	mutable ResourcePtr                        _obj;
	mutable Error                              _error;
	mutable bool                               _futurePulled{ false };
	mutable bool                               _isFinished{ false };

	bool _hasFuture{ false };
	bool _hasError{ false };
};
using Resource = RefPtr<Resource_>;

struct IResourceMaker
{
	virtual RefPtr<IResourceObject> Make() = 0;
};

template<class ResType>
struct SimpleResourceMaker : public IResourceMaker
{
	SimpleResourceMaker(class Application& app)
	: _app(app)
	{}

	virtual RefPtr<IResourceObject> Make()
	{
		return eastl::make_shared<ResType>(_app);
	}
	class Application& _app;
};

class ResourceMgr final
{
public:
	static const char* RootTask;

	using ResultType = Result<ResourcePtr, Error>;
	using FutureType = std::future<ResultType>;
	using AsyncType = tuple<tf::Task, FutureType>;
	using FuncType = function<ResultType()>;

	using TupleType = std::tuple<tf::Task, std::future<LoadResult>>;

	ResourceMgr(class Application& app);

	template<class ResType, class MakerType>
	void InstallMaker()
	{
		InstallMaker(ResType::MyType(), new MakerType(_app));
	}

	void InstallMaker(FireClassID resType, IResourceMaker* maker);

	template<class ResType>
	Resource QueueLoad(const char* filename)
	{
		if(!_tg.has(RootTask))
		{
			_tg.emplace([] {
				FIRE_LOG_DEBUG("!! Beginning ResourceMgr Load Root");
			}, RootTask);
		}
		auto[task, fut] = _tg.emplace(
			[&, this, fname = string(filename)] (tf::SubflowBuilder& builder) -> LoadResult {
				ResType::LoadOp loadOp(_app, fname.c_str(), eastl::make_shared<ResType>(_app));
				return loadOp(builder);
			}
		);
		_tg[RootTask].precede(task);
		using pair = eastl::pair<tf::Task, std::future<LoadResult>>;
		return eastl::make_shared<Resource_>(eastl::forward<pair>(eastl::make_pair(task, eastl::move(fut))));
	}

	Resource QueueLoad(FireClassID resType, const char* filename);

public:
	class Application& _app;
	class ObjectMaker& _objectMaker;
	class TaskGraph&   _tg;

	unordered_map<FireClassID, IResourceMaker*> _makers;
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