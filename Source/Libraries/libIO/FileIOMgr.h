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

/**
	\class FileIOMgr

	Manages the asynchronous loading of 
 **/
class FileIOMgr final
{
public:
	FileIOMgr();
	~FileIOMgr();

	EventDispatcher Dispatcher;

private:
	struct QueueItem
	{
		/**
			The type of resource object we will want to construct on successful load.
		 **/
		Mirror::Type ResourceType;
	};

	static void ThreadRun(SynchronizedQueue<QueueItem>* queue, EventDispatcher* dispatcher);

	SynchronizedQueue<QueueItem> _queue;
	Thread _threads[2] = {
		Thread(FileIOMgr::ThreadRun, &_queue, &Dispatcher),
		Thread(FileIOMgr::ThreadRun, &_queue, &Dispatcher)
	};
};

CLOSE_NAMESPACE(Firestorm);
#endif