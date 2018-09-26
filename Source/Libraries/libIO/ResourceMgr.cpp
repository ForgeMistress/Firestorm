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

ResourceMgr::LoadOp::LoadOp(ResourceLoader* loader, ResourceReference* ref, ResourceHandle handle)
: loader(loader)
, ref(ref)
, handle(handle)
#ifndef FIRE_FINAL
, filename(ref->GetResourcePath())
#endif
{
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

ResourceHandle ResourceMgr::Load(ResourceLoader* loader, ResourceReference* ref)
{
	// retrieve the resource from the cache first if it exists.
	const auto& path = ref->GetResourcePath();
	if(_cache.HasResource(path))
	{
		// and set the resource right now.
		return _cache.GetResource(path);
	}
	ResourceHandle outHandle = _cache.GetResource(path);
	// otherwise we're gonna have to load this sucker.

	LoadOp loadOp(loader, ref, outHandle);

	std::unique_lock<Mutex> lock(_lock);

	outHandle->SetState(ResourceHandleObject::State::kWaitingForLoad);

	_queue.push(std::move(loadOp));

	lock.unlock();
	_cv.notify_all();

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
	for(size_t i = 0; i < _loaders.size(); ++i)
	{
		delete _loaders[i].second;
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
	_loaders.push_back(std::make_pair(type, loader));
	return true;
}

ResourceLoader* ResourceMgr::GetLoader(const ResourceTypeID* type)
{
	for (size_t i = 0; i < _loaders.size(); ++i)
	{
		if(_loaders[i].first == type)
			return _loaders[i].second;
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

			oper.handle->SetState(ResourceHandleObject::State::kLoading);

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

					oper.handle->SetResourcePointer(resourcePointer);
					oper.handle->SetState(ResourceHandleObject::State::kLoaded);
				}
				else
				{
					oper.handle->SetError(result.error().GetCode());
					oper.handle->SetState(ResourceHandleObject::State::kLoadError);
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