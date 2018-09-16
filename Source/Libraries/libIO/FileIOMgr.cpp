///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FileIOMgr.h"

#include "File.h"

#include <physfs/physfs.h>
#include "ResourceReference.h"
#include <libIO/libIO.h>

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(FileLoadedEvent) {}
FIRE_MIRROR_DEFINE(FileLoadErrorEvent) {}

FileIOMgr::FileIOMgr()
{
}

FileIOMgr::~FileIOMgr()
{
}

void FileIOMgr::ThreadRun(SynchronizedQueue<QueueItem>* queue, EventDispatcher* dispatcher)
{
	while(true)
	{
		/*FileIOMgr::QueueItem item(queue->pop());
		Vector<char> data = libIO::LoadFile(item.Resource.GetResourcePath());*/
		dispatcher->Dispatch(FileLoadedEvent());
	}
}

CLOSE_NAMESPACE(Firestorm);