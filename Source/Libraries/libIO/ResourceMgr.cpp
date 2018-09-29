///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceMgr.h"

#include "ResourceReference.h"
#include "ResourceIOErrors.h"

#include <sstream>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadOp::LoadOp(ResourceLoader* l, ResourceReference* r, PromiseT* promise)
: loader(l)
, ref(r)
, promise(std::move(promise))
#ifndef FIRE_FINAL
, filename(ref->GetResourcePath())
#endif
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadOp::LoadOp(LoadOp&& other)
: loader(other.loader)
, ref(other.ref)
, promise(std::move(other.promise))
#ifndef FIRE_FINAL
, filename(std::move(other.filename))
#endif
{
	other.loader = nullptr;
	other.ref = nullptr;
	other.promise = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadOp::~LoadOp()
{
	delete promise;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::ResourceMgr()
{
	/*for(size_t i = 0; i < _numThreads; ++i)
	{
		std::stringstream ss;
		ss << "ResourceMgr Thread [" << i << "]";
		_threads[i] = Thread(std::bind(&ResourceMgr::ThreadRun, this));

		while(!_threads[i].joinable()); // wait until the thread is joinable.

		libCore::SetThreadName(_threads[i], ss.str());
	}*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::~ResourceMgr()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Future<ResourceLoader::LoadResult> ResourceMgr::Load(ResourceLoader* loader, ResourceReference* ref)
{
	// retrieve the resource from the cache first if it exists.
	// const auto& path = ref->GetResourcePath();

	return std::async(std::launch::async, [this, loader, ref]() -> ResourceLoader::LoadResult {
		FIRE_LOG_DEBUG("Loading Resource: %s", ref->GetResourcePath());
		if(_cache.HasResource(ref->GetResourcePath()))
		{
			return ResourceLoader::LoadResult(_cache.FindResource(ref->GetResourcePath()));
		}
		return loader->Load(this, *ref);
	});
}

	// otherwise we're gonna have to load this sucker.
	/*PromiseT* promise = new PromiseT;
	Future<ResourceLoader::LoadResult> future(promise->get_future());

	std::unique_lock<Mutex> lock(_lock);
	_queue.push(std::make_unique<LoadOp>(loader, ref, promise));

	lock.unlock();
	_cv.notify_all();

	// return future;
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceMgr::Shutdown()
{
	_quit = true;
	_cv.notify_all();

	for(size_t i = 0; i < _numThreads; i++)
	{
		if(_threads[i].joinable())
		{
			_threads[i].join();
		}
	}
	_loaders.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ResourceMgr::InstallLoader(const ResourceTypeID* type, ResourceLoader* loader)
{
	FIRE_ASSERT_MSG(_loaders.find(type) == _loaders.end(), "loader for type already installed");
	_loaders[type] = std::move(UniquePtr<ResourceLoader>(loader));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceLoader* ResourceMgr::GetLoader(const ResourceTypeID* type)
{
	auto found = _loaders.find(type);
	if(found != _loaders.end())
	{
		return found->second.get();
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RefPtr<ResourceHandle> ResourceMgr::GetResourceHandle(const String& resourceName) const
{
	RefPtr<IResourceObject> resourcePtr = _cache.FindResource(resourceName);
	if(resourcePtr)
	{
		return std::make_shared<ResourceHandle>(resourcePtr);
	}
	return std::make_shared<ResourceHandle>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*void ResourceMgr::ThreadRun()
{
	bool resourceThreadError = false;
	std::unique_lock lock(_lock);
	do
	{
		_cv.wait(lock, [this, &resourceThreadError] {
			return _queue.size() || _quit || resourceThreadError;
		});
		if(!_queue.empty() && !_quit && !resourceThreadError)
		{
			UniquePtr<LoadOp> oper(std::move(_queue.front()));
			_queue.pop();

			//oper.handle->SetState(ResourceHandleState::kLoading);

			FIRE_LOG_DEBUG("Loading %s", oper->ref->GetResourcePath());

			// unlock now that we're done messing with the queue.
			lock.unlock();

			auto loader = oper->loader;

			auto path = oper->ref->GetResourcePath();
			try
			{
				if(_cache.HasResource(path))
				{
					oper->promise->set_value(ResourceLoader::LoadResult(_cache.FindResource(path)));
				}
				else
				{
					auto result = loader->Load(this, *oper->ref);
					oper->promise->set_value(result);
				}
				/*auto result = loader->Load(this, *oper.ref);
				if(result.GetResource())
				{
					_cache.AddResource(oper.ref->GetResourcePath(), result.R);
					//oper.handle->SetResourcePointer(resourcePointer);
				}
				else
				{
					//oper.handle->SetError(result.error().GetCode());
				}
				oper.promise->set_value(result);
				oper.promise.release();
				//oper.handle->SetState(ResourceHandleState::kFinished);*/

				/*lock.lock();
			}
			catch(std::exception& e)
			{
				resourceThreadError = true;
				FIRE_LOG_ERROR("Error In Worker Thread: %s", e.what());
			}
		}
	} while(!_quit && !resourceThreadError);
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);