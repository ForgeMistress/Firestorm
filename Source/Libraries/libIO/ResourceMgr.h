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
#include <libCore/ObjectMaker.h>
#include <taskflow/taskflow.hpp>

#include <libCore/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

class ResourceLoader;

#define FIRE_RESOURCE_LOADOP_DECLARATION_                                                            \
struct LoadOp final : public IResourceObject::ILoadOp				                                 \
{																				                     \
	ResourceReference ResourceRef;												                     \
	class Application* App;														                     \
	class ResourceMgr* Mgr;                                                                          \
                                                                                                     \
	LoadOp();                                                                                        \
	class Application& GetApp() const;                                                               \
	virtual void Set(class Application* app, class ResourceMgr* mgr, const char* filename) override; \
	virtual void Operate() override;                                                                 \
	LoadResult DoOperation(Ptr& Resource);                                                           \
}

#define FIRE_RESOURCE_DECLARE_BASE(RES)                                                         \
private:                                                                                        \
	FIRE_MIRROR_DECLARE(RES);														            \
	friend class ResourceMgr;														            \
	using Ptr = eastl::shared_ptr<RES>;                                                         \
	using BaseType = RES;                                                                       \
	FIRE_RESOURCE_LOADOP_DECLARATION_;                                                          \
	static LoadOp _s_loadOp;                                                                    \
	virtual IResourceObject::ILoadOp* GetLoadOperation() const override { return &_s_loadOp; }  \
private:

#define FIRE_RESOURCE_DECLARE(RES, BASE)                                                        \
private:                                                                                        \
	FIRE_MIRROR_DECLARE(RES, BASE);												                \
	friend class ResourceMgr;														            \
	using Ptr = eastl::shared_ptr<RES>;                                                         \
	using BaseType = BASE;                                                                      \
	FIRE_RESOURCE_LOADOP_DECLARATION_;                                                          \
	static LoadOp _s_loadOp;                                                                    \
	virtual IResourceObject::ILoadOp* GetLoadOperation() const override { return &_s_loadOp; }  \
private:

#define FIRE_RESOURCE_LOADOP_CONSTRUCTOR(RES) \
RES::LoadOp RES::_s_loadOp;                   \
RES::LoadOp::LoadOp() {}                      \

#define FIRE_RESOURCE_LOADOP_GETAPP(RES) \
Application& RES::LoadOp::GetApp() const \
{										 \
	return *App;						 \
}

#define FIRE_RESOURCE_LOADOP_SET(RES)                                                       \
void RES::LoadOp::Set(class Application* app, class ResourceMgr* mgr, const char* filename) \
{                                                                                           \
	App = app;																		        \
	Mgr = mgr;																		        \
	ResourceRef = ResourceReference(filename);										        \
}																					        \

#define fire_cast(to,item) \
eastl::dynamic_pointer_cast<to>(item);

#define FIRE_RESOURCE_LOADOP_INVOKE(RES)                             \
void RES::LoadOp::Operate()								             \
{																	 \
	eastl::string path(ResourceRef.GetPath());                       \
	auto cache = Mgr->GetCache<RES::BaseType>();                     \
	auto casted = fire_cast(RES, cache->GetCachedPtr(path.c_str())); \
	LoadResult res(DoOperation(casted));                             \
}

#define FIRE_RESOURCE_LOADOP_DOOP(RES) \
LoadResult RES::LoadOp::DoOperation(Ptr& ResourcePtr)

#define FIRE_RESOURCE_DEFINE(RES)         \
	FIRE_RESOURCE_LOADOP_CONSTRUCTOR(RES) \
	FIRE_RESOURCE_LOADOP_GETAPP(RES)      \
	FIRE_RESOURCE_LOADOP_SET(RES)         \
	FIRE_RESOURCE_LOADOP_INVOKE(RES)      \
	FIRE_RESOURCE_LOADOP_DOOP(RES)

class ResourceMgr final
{
public:
	ResourceMgr(class Application& app);
	~ResourceMgr();

	/**
		Schedule a resource of the provided type and the supplied filename for load and return a Resource
		handle that can be used to track the progress of that load. For further details on tracking loads, see
		the Resource type itself.
	 **/
	template<class ResType, class... Args>
	Resource<ResType> Load(const char* filename, Args&&... args)
	{
		using PtrType = eastl::shared_ptr<ResType>;
		auto cache = GetCache<ResType>();
		if(cache->IsCached(filename))
		{
			FIRE_LOG_DEBUG("/!\\ RETURNING CACHED '%s' /!\\", filename);
			return Resource<ResType>(cache->GetCached(filename));
		}
		cache->CacheResourceInstance(filename, 
			_resourceMaker.Make<ResType>(
				_app, 
				eastl::forward<Args>(args)...
			)
		);
		auto objPtr = cache->GetCached(filename);

		IResourceObject::ILoadOp* loadOp = objPtr->GetLoadOperation();
		loadOp->Set(&_app, this, filename);
		loadOp->Operate();

		return Resource<ResType>(objPtr);
	}

	/**
		BaseType can either be the base type of the resource (if there's an inheritence hierarachy)
		or the type itself. If you need to establish a mapping between Base->Inherited, however,
		you'll need to have BaseType be your base type and SuperType be the actual concrete type
		that you want to be allocated.

		This is necessary to be able to abstract away the superclass implementations of some
		resource objects within libScene.
	 **/
	template<class Type, class F>
	bool AddResourceMaker(F&& func)
	{
		FireClassID type = Type::MyType();
		if(!_resourceMaker.IsMakerRegistered<Type>())
		{
			_resourceMaker.RegisterMaker<Type>(
				new Maker<Type>(eastl::forward<F>(func))
			);
			return true;
		}
		return false;
	}

	void CleanOldResources()
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		for(auto& cachePair : _caches)
		{
			cachePair.second->ClearUnusedResources();
		}
	}

	template<class T>
	ResourceCache<T>* GetCache() const
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		auto type = T::MyType();
		FIRE_ASSERT_MSG(_caches.find(type) != _caches.end(), "cache of specified type was not found");
		return reinterpret_cast<ResourceCache<T>*>(_caches[T::MyType()]);
	}

public:
	template<class T>
	void AddResourceCache()
	{
		std::unique_lock<std::mutex> lock(_cacheLock);
		auto type = T::MyType();
		if(_caches.find(type) == _caches.end())
		{
			_caches[type] = new ResourceCache<T>;
		}
	}

	template<class T>
	ResourcePtr MakeResource() const
	{
		std::unique_lock<std::mutex> lock(_makerLock);
		FIRE_ASSERT(_resourceMakers.find(T::MyType()) != _resourceMakers.end());
		return _resourceMakers[T::MyType()]->Make();
	}

	class Application& _app;

	ObjectMaker _resourceMaker;

	mutable std::mutex _cacheLock;
	mutable unordered_map<FireClassID, IResourceCache*> _caches;
};

CLOSE_NAMESPACE(Firestorm);
#endif