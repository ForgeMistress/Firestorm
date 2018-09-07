///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_FILEIOMGR_H_
#define LIBMIRROR_FILEIOMGR_H_
#pragma once

#include "File.h"

OPEN_NAMESPACE(Firestorm);

class FileIOMgr final
{
public:
	enum Errors : int
	{
		ERROR_FILE_ALREADY_QUEUED, //< The file is already queued for load.
		ERROR_FILE_ALREADY_LOADED  //< The file already has data and thus, is already loaded.
	};
	typedef RefPtr<File> FileHandle;

	enum QueueType
	{
		QUEUE_ASYNC,
		QUEUE_SYNC
	};

	typedef Function<void(const FileIOMgr*, FileHandle, Result<void, Error>)> ReadErrorCallback_f;
	typedef Function<void(const FileIOMgr*, FileHandle, Result<void, Error>)> WriteErrorCallback_f;

public:
	FileIOMgr();

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
	Result<void, Error> QueueFileForRead(FileHandle file, QueueType queueType, bool overwrite = false);
	bool CancelQueuedRead(FileHandle file);

	Result<void, Error> QueueFileForWrite(FileHandle file, QueueType queueType);
	bool CancelQueuedWrite(FileHandle file);


	struct ReadCallbackReceipt_;
	typedef RefPtr<ReadCallbackReceipt_> ReadCallbackReceipt;
	friend struct ReadCallbackReceipt_;

	struct WriteCallbackReceipt_;
	typedef RefPtr<WriteCallbackReceipt_> WriteCallbackReceipt;
	friend struct WriteCallbackReceipt_;

	ReadCallbackReceipt RegisterReadErrorCallback(ReadErrorCallback_f readCallback);
	WriteCallbackReceipt RegisterWriteErrorCallback(WriteErrorCallback_f readCallback);

	/**
		Retrieve the number of 
	 **/



private:
	void UnregisterReadErrorCallback(const ReadErrorCallback_f& callback);
	void UnregisterWriteErrorCallback(const WriteErrorCallback_f& callback);

	struct QueueEntry
	{
		FileHandle file;
		QueueType  type;
	};
	typedef List<QueueEntry> FileQueue;
	typedef List<QueueEntry> FileList;

	// pops out the first item in the list.
	QueueEntry PopFromFront(FileQueue& queue) const;

	// pushes a handle in while avoiding duplicates in the list.
	bool PushInBack(FileQueue& queue, const QueueEntry& entry);

	bool IsInQueue(FileQueue& queue, FileHandle file, QueueEntry& entry);

	FileQueue m_fileReadQueue;
	FileQueue m_fileWriteQueue;

	FileList m_filesBeingRead;
	FileList m_filesBeingWritten;

	List<ReadErrorCallback_f> m_readErrorCallbacks;
	List<WriteErrorCallback_f> m_writeErrorCallbacks;

	// Cache of files who have had handles returned already.
	mutable UnorderedMap<String, FileHandle> m_fileCache;
};

CLOSE_NAMESPACE(Firestorm);
#endif