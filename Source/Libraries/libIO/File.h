///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  File.h
//
//  A file that makes use of the pimpl idiom to implement a cross-platform abstraction of a file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_FILE_H_
#define LIBIO_FILE_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

class FileIOMgr;

class File final
{
	friend class FileIOMgr;

public:
	enum State
	{
		STATE_UNLOADED,          //< The File object default state. No data is loaded, but it knows the file on disk it's pointing to.
		STATE_UNLOADING,         //< The File object is in the process of unloading the data.
		STATE_QUEUED_FOR_READ,   //< The File object is queued to be read from disk.
		STATE_QUEUED_FOR_WRITE,  //< The File object is in the queue to have its data written to disk.
		STATE_READING_FROM_DISK, //< The File object is in the process of reading the data from disk.
		STATE_WRITING_TO_DISK,   //< The File object is in the process of writing the data to disk.
		STATE_LOADED             //< The File object has data loaded from disk.
	};
	enum Errors : int
	{
		ERROR_WRITE_IN_PROGRESS,
		ERROR_READ_IN_PROGRESS,
		ERROR_COULD_NOT_READ_DATA,
		ERROR_COULD_NOT_WRITE_DATA,
		ERROR_IMPROPER_STATE
	};
public:
	typedef Result<void, Error> WriteResult;
	typedef Result<DataBuffer, Error> ReadResult;
	typedef Function<void(File*, const WriteResult&)> AsyncWriteCallback_f;
	typedef Function<void(File*, const ReadResult&)>  AsyncReadCallback_f;

private:
	/**
		The constructor for a File object.
	 **/
	File(const FileIOMgr* fileIOMgr, const String& filename);

public:
	/**
		Check whether or not the file even exists.
	 **/
	bool Exists() const;

	/**
		Retrieve the name of the file (with extension) with absolutely zero path information.
	 **/
	const String& GetFilename() const;

	/**
		Retrieve the name of the file (without the extension) with absolutely zero path information.
	**/
	const String& GetFilenameWithoutExtension() const;

	/**
		Retrieve the extension on its own without a leading '.'
	 **/
	const String& GetFileExtension() const;

	/**
		Retrieve the absolute path on disk to this file.
	 **/
	const String& GetAbsolutePath() const;

	/**
		Retrieve the file size as the OS reports it.
	 **/
	uint16_t GetFileSize() const;

	/**
		Retrieve the size of the data if it is loaded into memory. Will return 0 if it is not loaded.
	 **/
	size_t GetDataSize() const;

	/**
		Retrieve whether or not the held data buffer is empty.
	 **/
	bool HasData() const;

	/**
		Retrieve the loaded data. return.empty() will be true if no data was loaded.
	 **/
	const DataBuffer& GetData() const;

	/**
		Retrieve the loaded data. return.empty() will be true if no data was loaded. Non-const version.
	**/
	DataBuffer& GetData();

	/**
		Retrieve the loading state of the file. Guaranteed atomic operation, so can be safely called across threads.
	 **/
	State GetState() const;

private:
	void SetState(State state);

	void ClearDataBuffer();

	Result<void, Error> InvokeWriteCallback(const WriteResult& result);
	Result<void, Error> InvokeReadCallback(const ReadResult& result);

	Result<void, Error> PerformDiskWriteSync();
	Result<void, Error> PerformDiskWriteAsync();
	Result<void, Error> PerformDiskReadSync();
	Result<void, Error> PerformDiskReadAsync();

private:
	// reference to the FileIOMgr that created this object.
	const FileIOMgr* m_mgr;

	// the data that has been loaded from disk, if any.
	DataBuffer m_data;

	// filename that was passed to the file object.
	String m_filename;

	mutable State m_state;

	mutable AsyncWriteCallback_f m_writeCallback;
	mutable AsyncWriteCallback_f m_readCallback;

	/**
		The implementation of the IO logic for each platform. Implement the following for each platform.
			* Constructor - Use implData for any platform specific data that is required.
			* PerformDiskRead
			* PerformDiskWrite
	 **/
	struct Impl
	{
		Impl();
		~Impl()
		{
			if(m_data) delete m_data;
		}

		/**
			Perform a synchronous disk read.
		 **/
		Result<DataBuffer, Error> PerformDiskReadSync(const String& filename);

		/**
			Perform a synchronous disk write.
		 **/
		Result<void, Error> PerformDiskWriteSync(const String& filename, const DataBuffer& data);

		/**
			Convenience function to cast the data to the platform specific type data.
		 **/
		template <class T>
		T* GetData() { return static_cast<T*>(m_data); }

		void* m_data;
	};
	std::unique_ptr<Impl> m_impl;
};

CLOSE_NAMESPACE(Firestorm);

#endif