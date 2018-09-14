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

#include <libMirror/EventDispatcher.h>
#include "File.h"

OPEN_NAMESPACE(Firestorm);

typedef RefPtr<class File> FileHandle;

class FileIOMgr final
{
public:
	enum Errors : int
	{
		ERROR_FILE_ALREADY_QUEUED, //< The file is already queued for load.
		ERROR_FILE_ALREADY_LOADED  //< The file already has data and thus, is already loaded.
	};

	typedef Function<void(const FileIOMgr*, FileHandle, Result<void, Error>)> ReadErrorCallback_f;
	struct QueueEntry
	{
		FileHandle file;
	};
	typedef List<QueueEntry> FileQueue;
	typedef List<QueueEntry> FileList;

	struct ThreadInfo
	{
		ThreadInfo(FileIOMgr* mgr);

		FileIOMgr* _mgr;
		Mutex _fileAsyncReadQueueMutex;
		FileQueue _fileAsyncReadQueue;

		Atomic<bool> _isProcessing;

		Mutex _fileCacheMutex;
		UnorderedMap<String, FileHandle> _fileCache;
	};

	struct ReadEvent
	{
		FIRE_MIRROR_DECLARE(ReadEvent);
	public:
		ReadEvent(FileHandle file, Result<Vector<char>, Error> result);
		FileHandle File;
		Result<Vector<char>, Error> Result;
	};
public:
	FileIOMgr();
	~FileIOMgr();

	/**
		Process the files that have been queued for read and write.
	 **/
	void ProcessQueues();

	/**
		Retrieve a handle to a file to be used in future operations.
	 **/
	FileHandle GetFile(const String& filename) const;

	/**
		Queue a file returned by \ref GetFile to be read from disk.
		\return result the result of the operation. Error can have 
	 **/
	Result<void, Error> QueueFileForSyncRead(FileHandle file, bool overwrite = false);
	Result<void, Error> QueueFileForAsyncRead(FileHandle file, bool overwrite = false);
	bool CancelQueuedSyncRead(FileHandle file);

	/**
		Retrieve the number of 
	 **/

	bool IsProcessing() const;

	void Shutdown();

	EventDispatcher Dispatcher;

private:
	void Initialize();

	// pops out the first item in the list.
	QueueEntry PopFromFront(FileQueue& queue) const;

	// pushes a handle in while avoiding duplicates in the list.
	bool PushInBack(FileQueue& queue, const QueueEntry& entry);

	bool IsInQueue(FileQueue& queue, FileHandle file, QueueEntry& entry);

	ThreadInfo* _threadInfo;

	FileQueue _fileSyncReadQueue;

	Thread _asyncReadThread;
};

CLOSE_NAMESPACE(Firestorm);
#endif