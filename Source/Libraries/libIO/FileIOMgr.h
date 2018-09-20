///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_FILEIOMGR_H_
#define LIBIO_FILEIOMGR_H_
#pragma once

#include <libCore/Expected.h>
#include <libCore/Result.h>
#include <libCore/SynchronizedQueue.h>
#include <libMirror/EventDispatcher.h>
#include <libMirror/ObjectMaker.h>
#include <libCore/libCore.h>
#include "ResourceReference.h"

#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);


struct ResourceIOErrors
{
	enum Errors : char
	{
		//< Return this error when the file could not be found.
		kFileNotFound,

		//< Return this if the file fails to load for some reason.
		kFileRead,

		//< Return this error when there's an error with parsing.
		kParsingException,
		kProcessingException
	};
};


/**
	Event that is dispatched when a file finishes loading.
 **/
struct ResourceLoadedEvent
{
	FIRE_MIRROR_DECLARE(ResourceLoadedEvent);

	ResourceLoadedEvent(const RefPtr<IResourceObject>& resource)
	: Resource(resource)
	{
	}

	RefPtr<IResourceObject> Resource;
};

/**
	Event that is dispatched when a file load encounters an error.
 **/
struct ResourceLoadErrorEvent
{
	FIRE_MIRROR_DECLARE(ResourceLoadErrorEvent);

	ResourceLoadErrorEvent(const ResourceReference& ref, const String& error)
	: Resource(ref)
	, Error(error)
	{
	}
	ResourceReference Resource;
	String Error;
};

class ResourceLoader
{
	FIRE_MIRROR_DECLARE(ResourceLoader);
public:
	ResourceLoader(const ResourceReference& ref);
	virtual ~ResourceLoader();

	RefPtr<IResourceObject> Load() const;

	const ResourceReference& GetResourceRef() const { return _resource; }

protected:
	ResourceReference _resource;
};

/**
	\class FileIOMgr

	Manages the asynchronous loading of 

	https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/dispatch.cpp
 **/
class FileIOMgr final
{
public:
	using Func_t = Function<void(void)>;

	FileIOMgr(ObjectMaker& objectMaker);
	~FileIOMgr();

	template <class ResourceType_t>
	void Load(const ResourceReference& ref)
	{
		Load(ref, ResourceType_t::MyType());
	}
	void Load(const ResourceReference& ref, Mirror::Type resourceLoaderType);

	/*void Load(const Func_t& loadFunctor);
	void Load(Func_t&& loadFunctor);*/

	void Shutdown();

	EventDispatcher Dispatcher;

private:
	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<ResourceLoader*> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	ObjectMaker& _objectMaker;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif