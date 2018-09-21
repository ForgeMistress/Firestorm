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
#include <libIO/libIO.h>
#include <sstream>

OPEN_NAMESPACE(Firestorm);

ErrorCode ResourceIOErrors::DEFAULT_LOADER("define a new loader for this type");
ErrorCode ResourceIOErrors::FILE_NOT_FOUND_ERROR("file was not found");
ErrorCode ResourceIOErrors::FILE_READ_ERROR("file could not be read");
ErrorCode ResourceIOErrors::PARSING_ERROR("there was an error while parsing the data in file");
ErrorCode ResourceIOErrors::PROCESSING_ERROR("there was an error while processing the file");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceLoader::ResourceLoader()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadResult ResourceLoader::Load(const ResourceReference&)
{
	return FIRE_ERROR(ResourceIOErrors::DEFAULT_LOADER);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::LoadOp::LoadOp(ResourceLoader* loader, ResourceReference* ref)
: loader(loader)
, ref(ref)
{
	ref->SetFuture(promise.get_future());
}

ResourceMgr::LoadOp::LoadOp(LoadOp&& other)
: loader(std::move(other.loader))
, ref(std::move(other.ref))
, promise(std::move(other.promise))
{
	other.loader = nullptr;
	other.ref = nullptr;
}

ResourceMgr::LoadOp& ResourceMgr::LoadOp::operator=(LoadOp&& other)
{
	loader = std::move(other.loader);
	ref = std::move(other.ref);
	promise = std::move(other.promise);

	other.loader = nullptr;
	other.ref = nullptr;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::ResourceMgr()
{
	for(size_t i = 0; i < _numThreads; ++i)
	{
		std::stringstream ss;
		ss << "ResourceMgr Thread [" << i << "]";
		_threads[i] = Thread(std::bind(&ResourceMgr::ThreadRun, this));
		libCore::SetThreadName(_threads[i], ss.str());
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::~ResourceMgr()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceMgr::Load(LoadOp* load)
{
	std::unique_lock lock(_lock);
	_queue.push(load);
	lock.unlock();
	_cv.notify_one();
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

bool ResourceMgr::InstallLoader(Mirror::Type type, ResourceLoader* loader)
{
	for(size_t i = 0; i < _loaders.size(); ++i)
	{
		if(_loaders[i].first == type)
			return false;
	}
	_loaders.push_back(std::make_pair(type, loader));
	return true;
}

ResourceLoader* ResourceMgr::GetLoader(Mirror::Type type)
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
			auto& oper = _queue.front();
			_queue.pop();

			// unlock now that we're done messing with the queue.
			lock.unlock();

			auto loader = oper->loader;
			auto& promise = oper->promise;

			oper->promise.set_value(loader->Load(*oper->ref));

			lock.lock();
			_opPool.Return(oper);
		}
	} while(!_quit);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);