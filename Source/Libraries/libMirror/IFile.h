///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IFile.h
//
//  An abstraction around a file on the filesystem.
//  included in SerialTraits.cpp for stdafx stuff.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_IFILE_H_
#define LIBMIRROR_IFILE_H_
#pragma once

OPEN_NAMESPACE(Elf);

class IFile
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
	/**
		Retrieve the name of the file on disk.
	 **/
	virtual const string& GetFilename() const = 0;

	/**
		Retrieve the extension of the file.
	 **/
	virtual const string& GetFileExtension() const = 0;

	/**
		Retrieve the full path to the file on disk.
	 **/
	virtual const string& GetPath() const = 0;

	/**
		Retrieve the size of the file on disk in bytes as the OS reports it.
	 **/
	virtual uint16_t GetFileSize() const = 0;

	/**
		Retrieve the size of the data in memory in bytes.
	 **/
	virtual size_t GetDataSize() const = 0;

	/**
		Retrieve a pointer to the data buffer.
	 **/
	virtual const char* GetData() const = 0;

	/**
		Write the data to disk synchronously.
	 **/
	virtual ResultCode WriteToDiskSync() const = 0;

	typedef std::function<void(IFile*, ResultCode)> WriteAsyncCallback;
	/**
		Write the data to disk asynchronously.
	**/
	virtual ResultCode WriteToDiskAsync(const WriteAsyncCallback& callback) const = 0;

	/**
		Read the data from disk synchronously.
	 **/
	virtual ResultCode ReadFromDiskSync() = 0;

	typedef std::function<void(IFile*, ResultCode)> ReadAsyncCallback;
	/**
		Read the data from disk asynchronously.
	**/
	virtual ResultCode ReadFromDiskAsync(const ReadAsyncCallback& callback) = 0;

	/**
		Retrieve the state of the file.
	 **/
	virtual State GetState() const = 0;
};
CLOSE_NAMESPACE(Elf);

#endif