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
	for(char i = 0; i < _numThreads; ++i)
	{
		_threads[i] = Thread(std::bind(&FileIOMgr::ThreadRun));
	}
}

FileIOMgr::~FileIOMgr()
{
	_quit = true;
	_cv.notify_all();

	for(char i = 0; i < _numThreads; i++)
	{
		if(_threads[i].joinable())
		{
			_threads[i].join();
		}
	}
}

void FileIOMgr::Load(const Func_t& loadFunctor)
{
	std::unique_lock lock(_lock);
	_queue.push(loadFunctor);
	lock.unlock();
	_cv.notify_all();
}

void FileIOMgr::Load(Func_t&& loadFunctor)
{
	std::unique_lock lock(_lock);
	_queue.push(std::move(loadFunctor));
	lock.unlock();
	_cv.notify_all();
}

void FileIOMgr::ThreadRun()
{
	std::unique_lock lock(_lock);
	do
	{
		_cv.wait(lock, [this] {
			return _queue.size() || _quit;
		});
		if(!_queue.empty() && !_quit)
		{
			auto op = std::move(_queue.front());
			_queue.pop();
			lock.unlock();

			op();

			lock.lock();
		}
	} while(!_quit);
}

CLOSE_NAMESPACE(Firestorm);