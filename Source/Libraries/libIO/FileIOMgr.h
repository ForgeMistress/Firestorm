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

template <class Resource_t>
struct ResourceLoader
{
	using load_function = Function<void(void)>;
	using resource_type = void;

};

/**
	\class FileIOMgr

	Manages the asynchronous loading of 
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
		Load();
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