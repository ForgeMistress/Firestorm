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
#include <libCore/libCore.h>

#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);

/**
	
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
	DefaultLoadFunctor(const ResourceReference& resourceReference)
		: _resourceReference(resourceReference)
	{
	}

	IResourceObject* operator()()
	{
		return nullptr;
	}

	const ResourceReference& _resourceReference;
};

/**
	Type traits object for resources.
 **/
template <class Resource_t>
struct ResourceLoader
{
	/**
		This should store the type of the functor that 
	 **/
	using load_functor = DefaultLoadFunctor;
	using resource_type = IResourceObject;
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

	FileIOMgr();
	~FileIOMgr();

	template <class T>
	void Load(const ResourceReference& ref)
	{
		static_assert(std::is_base_of<IResourceObject, T>::value)
		Load(std::bind(ResourceLoader<T>::load_functor(ref)));
	}

	void Load(const Func_t& loadFunctor);
	void Load(Func_t&& loadFunctor);

	EventDispatcher Dispatcher;

private:
	static const char _numThreads{ 2 };
	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<Func_t> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif