///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceMgr
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEMGR_H____
#define LIBIO_RESOURCEMGR_H____
#pragma once

#include <libCore/Expected.h>
#include <libCore/Result.h>
#include <libCore/SynchronizedQueue.h>
#include <libMirror/EventDispatcher.h>
#include <libMirror/ObjectMaker.h>
#include <libCore/libCore.h>
#include <libCore/ObjectPool.h>

#include <future>

#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);


struct ResourceIOErrors
{
	static ErrorCode DEFAULT_LOADER;
	static ErrorCode FILE_NOT_FOUND_ERROR;
	static ErrorCode FILE_READ_ERROR;
	static ErrorCode PARSING_ERROR;
	static ErrorCode PROCESSING_ERROR;
};

class ResourceLoader;

/**
	Specialize this to provide the data for the resource loader.
 **/
template<class T>
struct ResourceTraits
{
	using type = T;
	using loader = ResourceLoader;
};

class ResourceReference;

/**
	\class FileIOMgr

	Manages the asynchronous loading of 

	https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/dispatch.cpp
 **/
class ResourceMgr final
{
public:
	using LoadResult = Result<RefPtr<IResourceObject>, Error>;

private:
	using Task_t = std::function<LoadResult(const ResourceReference&)>;
	using Future_t = std::future<LoadResult>;
	using Promise_t = std::promise<LoadResult>;

	struct LoadOp
	{
		LoadOp(ResourceLoader* loader, ResourceReference* ref);
		LoadOp(LoadOp&& other);
		LoadOp& operator=(LoadOp&& other);

		LoadOp(const LoadOp&) = delete;
		LoadOp& operator=(const LoadOp&) = delete;

		ResourceLoader*    loader;
		ResourceReference* ref;
		Promise_t          promise;
	};
	ObjectPool<LoadOp> _opPool;
public:
	ResourceMgr();
	~ResourceMgr();

	template <class ResourceType_t>
	void Load(ResourceReference& ref)
	{
		ResourceLoader* loader = GetLoader(ResourceType_t::MyType());
		FIRE_ASSERT(loader && "no loader installed for this resource type");
		Load(_opPool.Get(loader, &ref));
	}
	void Load(LoadOp* loadOp);

	template<class Res_t>
	bool InstallLoader(ResourceLoader* loader)
	{
		return InstallLoader(Res_t::MyType(), loader);
	}

	void Shutdown();

private:
	bool InstallLoader(Mirror::Type type, ResourceLoader* loader);
	ResourceLoader* GetLoader(Mirror::Type type);

	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<LoadOp*> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	Vector<std::pair<Mirror::Type, ResourceLoader*>> _loaders;

	void ThreadRun();
};

class ResourceLoader
{
public:
	ResourceLoader();
	virtual ~ResourceLoader() {}

	virtual ResourceMgr::LoadResult Load(const ResourceReference& ref);
};

CLOSE_NAMESPACE(Firestorm);
#endif