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


struct FileIOMgr::ReadCallbackReceipt_
{
	ReadCallbackReceipt_(const RefPtr<FileIOMgr>& mgr, FileIOMgr::ReadErrorCallback_f callback)
	: m_mgr(mgr)
	, m_callback(callback)
	{
	}

	~ReadCallbackReceipt_()
	{
		if(auto mgr = m_mgr.Lock())
			mgr->UnregisterReadErrorCallback(m_callback);
	}
	WeakPtr<FileIOMgr>             m_mgr;
	FileIOMgr::ReadErrorCallback_f m_callback;
};

struct FileIOMgr::WriteCallbackReceipt_
{
	WriteCallbackReceipt_(const RefPtr<FileIOMgr>& mgr, FileIOMgr::WriteErrorCallback_f callback)
	: m_mgr(mgr)
	, m_callback(callback)
	{
	}

	~WriteCallbackReceipt_()
	{
		if(auto mgr = m_mgr.Lock())
			mgr->UnregisterReadErrorCallback(m_callback);
	}
	WeakPtr<FileIOMgr>              m_mgr;
	FileIOMgr::WriteErrorCallback_f m_callback;
};


FileIOMgr::FileIOMgr()
{
}

void FileIOMgr::ProcessQueues()
{
	if(!m_fileReadQueue.empty())
	{
		auto entry = m_fileReadQueue.front();
		m_fileReadQueue.pop_front();
		if(entry.type == QUEUE_SYNC)
		{
			Result<void, Error> result = entry.file->PerformDiskReadSync();

		}
	}
}

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

	if(!PushInBack(m_fileReadQueue, QueueEntry{ file, queueType }))
	{
		return ELF_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + filename + "' is already queued for read");
	}
	return ELF_RESULT(void);
}

bool FileIOMgr::CancelQueuedRead(FileHandle file)
{
	bool removed = false;
	m_fileReadQueue.remove_if([&file, &removed](const QueueEntry& entry) {
		if(entry.file == file)
		{
			removed = true;
			return true;
		}
		return false;
	});
	return removed;
}

Result<void, Error> FileIOMgr::QueueFileForWrite(FileHandle file, QueueType queueType)
{
	auto findFunction = [&file](const QueueEntry& entry) {
		return entry.file == file;
	};

	auto foundRead = std::find_if(m_fileReadQueue.begin(), m_fileReadQueue.end(), findFunction);
	if(foundRead != m_fileReadQueue.end())
	{
		return ELF_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + file->GetFilename() + "' already queued for read in call to QueueFileForWrite");
	}

	auto foundWrite = std::find_if(m_fileWriteQueue.begin(), m_fileWriteQueue.end(), findFunction);
	if(foundWrite != m_fileWriteQueue.end())
	{
		return ELF_ERROR(ERROR_FILE_ALREADY_QUEUED, "file with name '" + file->GetFilename() + "' already queued for write in call to QueueFileForWrite");
	}

	m_fileWriteQueue.push_back(QueueEntry{ file, queueType });
	return ELF_RESULT(void);
}

bool FileIOMgr::CancelQueuedWrite(FileHandle file)
{
	bool removed = false;
	m_fileWriteQueue.remove_if([&file, &removed](const QueueEntry& entry) {
		if(entry.file == file)
		{
			removed = true;
			return true;
		}
		return false;
	});
	return removed;
}

FileIOMgr::ReadCallbackReceipt FileIOMgr::RegisterReadErrorCallback(ReadErrorCallback_f callback)
{
	/*if(std::find(m_readErrorCallbacks.begin(), m_readErrorCallbacks.end(), callback) == m_readErrorCallbacks.end())
	{
		return std::make_shared<ReadCallbackReceipt_>(SharedPtr<FileIOMgr>(this), callback);
	}*/
	return nullptr;
}

FileIOMgr::WriteCallbackReceipt FileIOMgr::RegisterWriteErrorCallback(WriteErrorCallback_f callback)
{
	/*if(std::find(m_writeErrorCallbacks.begin(), m_writeErrorCallbacks.end(), callback) == m_readErrorCallbacks.end())
	{
		return std::make_shared<WriteCallbackReceipt_>(SharedPtr<FileIOMgr>(this), callback);
	}*/
	return nullptr;
}

void FileIOMgr::UnregisterReadErrorCallback(const ReadErrorCallback_f& callback)
{
	//m_readErrorCallbacks.remove(callback);
}

void FileIOMgr::UnregisterWriteErrorCallback(const WriteErrorCallback_f& callback)
{
	//m_writeErrorCallbacks.remove(callback);
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

CLOSE_NAMESPACE(Elf);