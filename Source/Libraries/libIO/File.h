///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File.h
//
//  A file that makes use of the pimpl idiom to implement a cross-platform abstraction of a file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_FILE_H_
#define LIBMIRROR_FILE_H_
#pragma once

OPEN_NAMESPACE(Elf);

class File final
{
public:
	enum State
	{
		UNLOADED,
		UNLOADING,
		LOADING_FROM_DISK,
		LOAD_ERROR,
		LOADED_AND_READY
	};
public:
	typedef std::function<void(File*, const Result&)> AsyncCallback;

public:
	File(const string& filename);

	/**
		Check whether or not the file even exists.
	 **/
	bool Exists() const;

	/**
		Retrieve the name of the file (with extension) with absolutely zero path information.
	 **/
	const string& GetFilename() const;

	/**
		Retrieve the name of the file (without the extension) with absolutely zero path information.
	**/
	const string& GetFilenameWithoutExtension() const;

	/**
		Retrieve the extension on its own without a leading '.'
	 **/
	const string& GetFileExtension() const;

	/**
		Retrieve the absolute path on disk to this file.
	 **/
	const string& GetPath() const;

	/**
		Retrieve the file size as the OS reports it.
	 **/
	uint16_t GetFileSize() const;

	/**
		Retrieve the size of the data if it is loaded into memory. Will return 0 if it is not loaded.
	 **/
	size_t GetDataSize() const;

	/**
		Retrieve a pointer to the underlying data. Will return nullptr if the data is not loaded.
	 **/
	const char* GetData() const;

	/**
		Synchronously write the data in memory to disk.
	 **/
	ResultCode WriteToDiskSync() const;

	/**
		Asynchronously write the data in memory to disk, calling AsyncCallback when complete.
	 **/
	ResultCode WriteToDiskAsync(const AsyncCallback& callback) const;

	/**
		Synchronously read the data on disk to memory.
	 **/
	ResultCode ReadFromDiskSync();

	/**
		Asynchronously read the data from disk, calling AsyncCallback when complete.
	 **/
	ResultCode ReadFromDiskAsync(const AsyncCallback& callback);

	/**
		Retrieve the loading state of the file. Guaranteed atomic operation, so can be safely called across threads.
	 **/
	State GetState() const;

private:
	struct FileImpl;
	std::unique_ptr<FileImpl> m_impl;
};

CLOSE_NAMESPACE(Elf);

#endif