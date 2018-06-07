///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File.h
//
//  Windows implementation of IFile.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_FILE_H_
#define LIBMIRROR_FILE_H_
#pragma once

OPEN_NAMESPACE(Elf);

class File
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

	bool Exists() const;

	const string& GetFilename() const;
	const string& GetFileExtension() const;
	const string& GetPath() const;
	uint16_t GetFileSize() const;
	size_t GetDataSize() const;
	const char* GetData() const;
	ResultCode WriteToDiskSync() const;
	ResultCode WriteToDiskAsync(const AsyncCallback& callback) const;
	ResultCode ReadFromDiskSync();
	ResultCode ReadFromDiskAsync(const AsyncCallback& callback);
	State GetState() const;

private:
	struct FileImpl;
	std::unique_ptr<FileImpl> m_impl;
};

CLOSE_NAMESPACE(Elf);

#endif