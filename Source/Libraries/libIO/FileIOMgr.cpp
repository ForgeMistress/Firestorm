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

OPEN_NAMESPACE(Elf);

FileIOMgr::FileIOMgr()
{
}

void FileIOMgr::ProcessQueues()
{
	if(!m_fileReadQueue.empty())
	{

	}
}
	/*if(m_filesBeingRead)
	{
		m_fileBeingRead = PopFromFront(m_fileReadQueue);
		if(m_fileBeingRead)
		{
			if(m_fileBeingRead->PerformDiskReadAsync())
			{

			}
		}
	}
	else
	{

	}
}*/

FileIOMgr::FileHandle FileIOMgr::GetFile(const String& filename) const
{
	if(m_fileCache.find(filename) == m_fileCache.end())
	{
		FileHandle file(new File(this, filename));
		m_fileCache[filename] = file;
	}
	return m_fileCache[filename];
}

Result<void, Error> FileIOMgr::QueueFileForRead(FileHandle file, QueueType queueType, bool overwrite)
{
	const String& filename = file->GetFilename();

	// If we're allowing overwrites, then clear the internal data buffer.
	if(overwrite)
	{
		file->ClearDataBuffer();
	}
	else
	{
		if(m_fileCache.find(filename) != m_fileCache.end())
		{
			if(m_fileCache[filename]->GetState() == File::STATE_LOADED)
			{
				// It's already loaded.
				return ELF_ERROR(ERROR_FILE_ALREADY_LOADED, "file with name '" + filename + "' is already loaded");
			}
		}
	}

	if(!PushInBack(m_fileReadQueue, file))
	{
		return ELF_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + filename + "' is already queued for read");
	}
	return ELF_RESULT(void);
}

bool FileIOMgr::CancelQueuedRead(FileHandle file)
{
	return false;
}

Result<void, Error> FileIOMgr::QueueFileForWrite(FileHandle file, QueueType queueType)
{
	return ELF_RESULT(void);
}

bool FileIOMgr::CancelQueuedWrite(FileHandle file)
{
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

bool FileIOMgr::PushInBack(FileQueue& queue, FileHandle file)
{
	for(auto f : queue)
	{
		if(f == file)
			return false;
	}
	m_fileReadQueue.push_back(file);
	return true;
}

bool FileIOMgr::IsInQueue(FileQueue& queue, FileHandle file)
{
	for(auto f : queue)
	{
		if(f == file)
		{
			return true;
		}
	}
	return false;
}

CLOSE_NAMESPACE(Elf);