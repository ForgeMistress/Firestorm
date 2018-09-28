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

ResourceMgr::LoadOp::LoadOp(ResourceLoader* l, ResourceReference r, RefPtr<ResourceHandle> h)
: loader(l)
, ref(r)
, handle(h)
#ifndef FIRE_FINAL
, filename(ref.GetResourcePath())
#endif
{
#ifndef FIRE_FINAL
	handle->SetFilename(ref.GetResourcePath());
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadOp::~LoadOp()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::ResourceMgr()
{
	for(size_t i = 0; i < _numThreads; ++i)
	{
		std::stringstream ss;
		ss << "ResourceMgr Thread [" << i << "]";
		_threads[i] = Thread(std::bind(&ResourceMgr::ThreadRun, this));

		while(!_threads[i].joinable()); // wait until the thread is joinable.

		libCore::SetThreadName(_threads[i], ss.str());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::~ResourceMgr()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RefPtr<ResourceHandle> ResourceMgr::Load(ResourceLoader* loader, ResourceReference ref)
{
	// retrieve the resource from the cache first if it exists.
	const auto& path = ref.GetResourcePath();
	if(_cache.HasResource(path))
	{
		// and set the resource right now.
		return _cache.GetResource(path);
	}
	// otherwise we're gonna have to load this sucker.

	std::unique_lock<Mutex> lock(_lock);
	_queue.push(LoadOp(loader, ref, std::move(_cache.GetResource(path))));

	lock.unlock();
	_cv.notify_all();

	return _queue.back().handle;
}

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
	for(size_t i = 0; i < _loaders.size(); ++i)
	{
		if(_loaders[i].first == type)
			return false;
	}
	_loaders.push_back(std::make_pair(type, std::make_unique<ResourceLoader>(loader)));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceLoader* ResourceMgr::GetLoader(const ResourceTypeID* type)
{
	for (size_t i = 0; i < _loaders.size(); ++i)
	{
		if(_loaders[i].first == type)
			return _loaders[i].second.get();
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceMgr::ThreadRun()
{
	bool resourceThreadError = false;
	std::unique_lock lock(_lock);
	do
	{
		_cv.wait(lock, [this, &resourceThreadError] {
			return _queue.size() || _quit || resourceThreadError;
		});
		if(!_queue.empty() && !_quit)
		{
			auto& oper = std::move(_queue.front());
			_queue.pop();

			oper.handle->SetState(ResourceHandleState::kLoading);

			FIRE_LOG_DEBUG("Loading %s", oper.filename);

			// unlock now that we're done messing with the queue.
			lock.unlock();

			auto loader = oper.loader;

			try
			{
				auto result = loader->Load(this, *oper.ref);
				if(result.has_value())
				{
					IResourceObject* resourcePointer = result.value();
					_cache.AddResource(oper.ref->GetResourcePath(), resourcePointer);

					oper.handle->Get()->SetResourcePointer(resourcePointer);
					oper.handle->Get()->SetState(ResourceHandleState::kLoaded);
				}
				else
				{
					oper.handle->Get()->SetError(result.error().GetCode());
					oper.handle->Get()->SetState(ResourceHandleState::kLoadError);
				}

				lock.lock();
			}
			catch(std::exception& e)
			{
				resourceThreadError = true;
				FIRE_LOG_ERROR("Error In Worker Thread: %s", e.what());
			}
		}
	} while(!_quit && !resourceThreadError);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);