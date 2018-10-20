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

#include <libApp/Application.h>

#include <sstream>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_ERRORCODE_DEF(ResourceHandleErrors::NULL_RESOURCE,
	"a null resource was passed to the constructor of the Resource");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource_::Resource_(eastl::pair<tf::Task, std::future<LoadResult>>&& tuppy)
: _tuppy(std::forward<eastl::pair<tf::Task, std::future<LoadResult>>>(tuppy))
, _hasFuture(true)
{
}

Resource_::~Resource_()
{
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error Resource_::GetError() const
{
	return _error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource_::IsValid() const
{
	return _hasFuture;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource_::IsFinished() const
{
	if(!_isFinished)
	{
		try
		{
			if(!_futurePulled)
			{
				auto status = get<1>(_tuppy).wait_for(std::chrono::milliseconds(0));
				if(status == std::future_status::ready)
				{
					_isFinished = true;
					auto loadResult = get<1>(_tuppy).get();
					_obj = loadResult.GetResource();
					_error = loadResult.GetError();

					_futurePulled = true;
				}
			}
		}
		catch(std::exception& e)
		{
			FIRE_LOG_ERROR("Exception in Resource::IsFinished: %s", e.what());
		}
	}
	return _isFinished;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource_::HasError() const
{
	return _error.GetCode() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Resource_::Release()
{
	_obj = nullptr;
	_error.Set(nullptr, "");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Resource_::DoPull() const
{
	// check the status of the future.
	auto status = get<1>(_tuppy).wait_for(std::chrono::milliseconds(0));
	if(status == std::future_status::ready)
	{
		_isFinished = true;
		auto loadResult = get<1>(_tuppy).get();
		_obj = loadResult.GetResource();
		_error = loadResult.GetError();

		_futurePulled = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourcePtr Resource_::PullData() const
{
	if(get<1>(_tuppy).valid())
	{
		if(!_futurePulled)
		{
			DoPull();
		}
	}

	return _obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ResourceMgr::RootTask = "ResourceMgr::RootTask";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::ResourceMgr(Application& app)
: _app(app)
, _objectMaker(app.GetSystems().ObjectMaker())
, _tg(app.GetSystems().TaskGraph())
{
}

void ResourceMgr::InstallMaker(FireClassID resType, IResourceMaker* maker)
{
	if(_makers.find(resType) == _makers.end())
	{
		_makers[resType] = maker;
	} else
		delete maker;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//ResourceMgr::ResourceMgr()
//{
//	for(size_t i = 0; i < _numThreads; ++i)
//	{
//		string ostring;
//		ostring.append_sprintf("ResourceMgr Thread[%d]", i);
//		_threads[i] = thread(std::bind(&ResourceMgr::ThreadRun, this));
//
//		while(!_threads[i].joinable()); // wait until the thread is joinable.
//
//		libCore::SetThreadName(_threads[i], ostring);
//	}
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//ResourceMgr::~ResourceMgr()
//{
//	Shutdown();
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void ResourceMgr::Shutdown()
//{
//	_quit = true;
//	_cv.notify_all();
//
//	for(size_t i = 0; i < _numThreads; i++)
//	{
//		if(_threads[i].joinable())
//		{
//			_threads[i].join();
//		}
//	}
//	_loaders.clear();
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//bool ResourceMgr::InstallLoader(const ResourceTypeID* type, ResourceLoader* loader)
//{
//	FIRE_ASSERT_MSG(_loaders.find(type) == _loaders.end(), "loader for type already installed");
//	_loaders[type] = std::move(UniquePtr<ResourceLoader>(loader));
//	return true;
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//ResourceLoader* ResourceMgr::GetLoader(const ResourceTypeID* type)
//{
//	auto found = _loaders.find(type);
//	if(found != _loaders.end())
//	{
//		return found->second.get();
//	}
//	return nullptr;
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Resource ResourceMgr::Load(ResourceLoader* loader, const ResourceReference& ref)
//{
//	PromiseT* promise = new PromiseT;
//
//	auto loadOperation = [this, loader, ref, promise](){
//		auto path = ref.GetResourcePath();
//		FIRE_LOG_DEBUG("Loading Resource: %s", path.c_str());
//		if(_cache.HasResource(path))
//		{
//			promise->set_value(_cache.FindResource(path));
//			delete promise;
//			return;
//		}
//		ResourceLoader::LoadResult result = loader->Load(this, ref);
//		if(!result.HasError())
//		{
//			_cache.AddResource(path, result.GetResource());
//		}
//		promise->set_value(std::move(result));
//		delete promise;
//	};
//
//	std::unique_lock<std::mutex> lock(_queueLock);
//	_queue.push(loadOperation);
//
//	lock.unlock();
//	_cv.notify_all();
//
//	return Resource(promise->get_future());
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void ResourceMgr::ThreadRun()
//{
//	std::unique_lock lock(_queueLock);
//	do
//	{
//		_cv.wait(lock, [this] {
//			return _queue.size() || _quit;
//		});
//		if(!_queue.empty() && !_quit)
//		{
//			auto func = std::move(_queue.front());
//			_queue.pop();
//			lock.unlock();
//
//			func();
//
//			lock.lock();
//		}
//	} while(!_quit);
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);