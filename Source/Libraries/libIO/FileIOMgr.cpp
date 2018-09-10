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

OPEN_NAMESPACE(Firestorm);

namespace {
	static void FileIOMgr_AsyncThread(FileIOMgr::ThreadInfo* info)
	{
		while(info->_isProcessing)
		{
			info->_fileAsyncReadQueueMutex.lock();
			if(!info->_fileAsyncReadQueue.empty())
			{
				FileIOMgr::QueueEntry entry(info->_fileAsyncReadQueue.back());
				info->_fileAsyncReadQueue.pop_back();
				info->_fileAsyncReadQueueMutex.unlock();

				// load the file
				Result<void,Error> result = entry.file->PerformDiskReadSync();
				if(result.has_value())
				{
					info->_mgr->Dispatcher.Dispatch(FileIOMgr::ReadEvent{ 
						entry.file,
						FIRE_RESULT(Vector<char>, entry.file->GetData())
					});
				}
				else
				{
					info->_mgr->Dispatcher.Dispatch(FileIOMgr::ReadEvent{
						entry.file,
						FIRE_ERROR(result.error().Code, result.error().Message)
					});
				}
			}
			else
			{
				info->_fileAsyncReadQueueMutex.unlock();
			}
		}
	}
}

FileIOMgr::ThreadInfo::ThreadInfo(FileIOMgr* mgr)
: _mgr(mgr)
, _isProcessing(true)
{
}

FileIOMgr::ReadEvent::ReadEvent(FileHandle file, Firestorm::Result<Vector<char>, Error> result)
: File(file)
, Result(result)
{
}

FileIOMgr::FileIOMgr()
: _threadInfo(new ThreadInfo(this))
, _asyncReadThread(FileIOMgr_AsyncThread, _threadInfo)
{
	Initialize();
}

FileIOMgr::~FileIOMgr()
{
	Shutdown();
	delete _threadInfo;
}

void FileIOMgr::ProcessQueues()
{
	if(_threadInfo->_isProcessing)
	{
		if(!_fileSyncReadQueue.empty())
		{
			auto entry = _fileSyncReadQueue.front();
			_fileSyncReadQueue.pop_front();
			Result<void, Error> result = entry.file->PerformDiskReadSync();
			if (result.has_value())
			{
				entry.file->SetState(File::STATE_LOADED);
			}
		}
	}
}

FileHandle FileIOMgr::GetFile(const String& filename) const
{
	std::scoped_lock lock(_threadInfo->_fileCacheMutex);
	if(_threadInfo->_fileCache.find(filename) == _threadInfo->_fileCache.end())
	{
		FileHandle file(new File(this, filename));
		_threadInfo->_fileCache[filename] = file;
	}
	return _threadInfo->_fileCache[filename];
}

Result<void, Error> FileIOMgr::QueueFileForSyncRead(FileHandle file, bool overwrite)
{
	const String& filename = file->GetFilename();

	// If we're allowing overwrites, then clear the internal data buffer.
	if(overwrite)
	{
		file->ClearDataBuffer();
	}
	else
	{
		std::scoped_lock lock(_threadInfo->_fileCacheMutex);
		if(_threadInfo->_fileCache.find(filename) != _threadInfo->_fileCache.end())
		{
			if(_threadInfo->_fileCache[filename]->GetState() == File::STATE_LOADED)
			{
				// It's already loaded.
				return FIRE_ERROR(ERROR_FILE_ALREADY_LOADED, "file with name '" + filename + "' is already loaded");
			}
		}
	}

	if(!PushInBack(_fileSyncReadQueue, QueueEntry{ file }))
	{
		return FIRE_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + filename + "' is already queued for read");
	}
	return FIRE_RESULT(void);
}

Result<void, Error> FileIOMgr::QueueFileForAsyncRead(FileHandle file, bool overwrite)
{
	const String& filename = file->GetFilename();

	// If we're allowing overwrites, then clear the internal data buffer.
	if(overwrite)
	{
		file->ClearDataBuffer();
	}
	else
	{
		std::scoped_lock lock(_threadInfo->_fileCacheMutex);
		if(_threadInfo->_fileCache.find(filename) != _threadInfo->_fileCache.end())
		{
			if(_threadInfo->_fileCache[filename]->GetState() == File::STATE_LOADED)
			{
				// It's already loaded.
				return FIRE_ERROR(ERROR_FILE_ALREADY_LOADED, "file with name '" + filename + "' is already loaded");
			}
		}
	}

	std::scoped_lock lock(_threadInfo->_fileAsyncReadQueueMutex);
	if(!PushInBack(_threadInfo->_fileAsyncReadQueue, QueueEntry{ file }))
	{
		return FIRE_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + filename + "' is already queued for read");
	}
	return FIRE_RESULT(void);
}

bool FileIOMgr::CancelQueuedSyncRead(FileHandle file)
{
	bool removed = false;
	_fileSyncReadQueue.remove_if([&file, &removed](const QueueEntry& entry) {
		if(entry.file == file)
		{
			removed = true;
			return true;
		}
		return false;
	});
	return removed;
}

bool FileIOMgr::IsProcessing() const
{
	return _threadInfo->_isProcessing;
}

void FileIOMgr::Shutdown()
{
	_threadInfo->_isProcessing = false;
	_asyncReadThread.join();
	_fileSyncReadQueue.clear();
}

void FileIOMgr::Initialize()
{
	_threadInfo->_isProcessing = true;
}

FileIOMgr::QueueEntry FileIOMgr::PopFromFront(FileQueue& queue) const
{
	if(!queue.empty())
	{
		QueueEntry entry(queue.front());
		queue.pop_front();
		return entry;
	}
	return QueueEntry();
}

bool FileIOMgr::PushInBack(FileQueue& queue, const QueueEntry& entry)
{
	for(auto f : queue)
	{
		if(f.file == entry.file)
			return false;
	}
	queue.push_back(entry);
	return true;
}

bool FileIOMgr::IsInQueue(FileQueue& queue, FileHandle file, QueueEntry& entry)
{
	for(auto f : queue)
	{
		if(f.file == file)
		{
			entry = f;
			return true;
		}
	}
	return false;
}

CLOSE_NAMESPACE(Firestorm);