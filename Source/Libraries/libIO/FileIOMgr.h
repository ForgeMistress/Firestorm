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
struct FileLoadedEvent
{
	FIRE_MIRROR_DECLARE(FileLoadedEvent);

	FileLoadedEvent(const RefPtr<IResourceObject>& resource)
		: Resource(resource)
	{
	}

	RefPtr<IResourceObject> Resource;
};

/**
	Event that is dispatched when a file load encounters an error.
 **/
struct FileLoadErrorEvent
{
	FIRE_MIRROR_DECLARE(FileLoadErrorEvent);

	FileLoadErrorEvent(const String& error)
		: Error(error)
	{
	}
	String Error;
};

struct DefaultLoadFunctor
{
	DefaultLoadFunctor(const ResourceReference& resourceReference);
	Result<RefPtr<IResourceObject>, Error> operator()();
	const ResourceReference& _resourceReference;
};

/**
	Type traits object for resources.
 **/
template <class Resource_t>
struct ResourceLoader
{
	/**
		This should store the type of the functor that will be used to load the resource.

		The load functor should implement the following.

		ctor(ResourceReference*, args...) with the functor making use of the resource reference
		to load the resource.

		Result<RefPtr<IResourceObject>, Error> operator(), which actually performs the logic of
		loading the resource.
	 **/
	using load_type = DefaultLoadFunctor;

	/**
		The top level type of the resource that is being returned. Convenience mostly.
	 **/
	using resource_type = Resource_t;
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

	template <class T, class... Args>
	void Load(ResourceReference* ref, Args... args)
	{
		static_assert(std::is_base_of<IResourceObject, T>::value)
		Load(std::bind(ResourceLoader<T>::load_type(ref, args...)));
	}

	void Load(const Func_t& loadFunctor);
	void Load(Func_t&& loadFunctor);

	void Shutdown();

	EventDispatcher Dispatcher;

private:
	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<Func_t> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	ObjectMaker& _objectMaker;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif