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

Resource ResourceMgr::Load(ResourceLoader* loader, const ResourceReference& ref)
{
	PromiseT* promise = new PromiseT;

	auto loadOperation = [this, loader, ref, promise](){
		auto path = ref.GetResourcePath();
		FIRE_LOG_DEBUG("Loading Resource: %s", path);
		if(_cache.HasResource(path))
		{
			promise->set_value(_cache.FindResource(path));
			delete promise;
			return;
		}
		ResourceLoader::LoadResult result = loader->Load(this, ref);
		if(!result.HasError())
		{
			_cache.AddResource(path, result.GetResource());
		}
		promise->set_value(std::move(result));
		delete promise;
	};

	std::unique_lock<Mutex> lock(_queueLock);
	_queue.push(loadOperation);

	lock.unlock();
	_cv.notify_all();

	return Resource(promise->get_future());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceMgr::ThreadRun()
{
	std::unique_lock lock(_queueLock);
	do
	{
		_cv.wait(lock, [this] {
			return _queue.size() || _quit;
		});
		if(!_queue.empty() && !_quit)
		{
			auto func = std::move(_queue.front());
			_queue.pop();
			lock.unlock();

			func();

			lock.lock();
		}
	} while(!_quit);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);