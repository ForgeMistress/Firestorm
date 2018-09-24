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

ResourceMgr::LoadOp::LoadOp(ResourceLoader* loader, ResourceReference* ref)
: loader(loader)
, ref(ref)
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
	// std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::~ResourceMgr()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceMgr::Load(LoadOp&& load)
{
	std::unique_lock lock(_lock);
	_queue.push(std::move(load));
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
	std::unique_lock lock(_lock);
	do
	{
		_cv.wait(lock, [this] {
			return _queue.size() || _quit;
		});
		if(!_queue.empty() && !_quit)
		{
			auto& oper = std::move(_queue.front());
			_queue.pop();

			FIRE_LOG_DEBUG("Loading %s", oper.filename);

			// unlock now that we're done messing with the queue.
			lock.unlock();

			auto loader = oper.loader;

			try
			{
				auto result = loader->Load(this, *oper.ref);
				if(result.has_value())
				{
					auto resultValue = result.value();
					oper.ref->SetResource(resultValue.first, resultValue.second);
				}
				else
				{
					oper.ref->SetError(result.error());
				}
			}
			catch(std::exception& e)
			{
				FIRE_LOG_ERROR("Error In Worker Thread: %s", e.what());
			}

			lock.lock();
		}
	} while(!_quit);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);