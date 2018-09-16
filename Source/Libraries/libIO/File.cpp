
#include "stdafx.h"
#include "File.h"

#include <physfs/physfs.h>

OPEN_NAMESPACE(Firestorm);

File::File(const FileIOMgr* fileIOMgr, const String& filename)
: _mgr(fileIOMgr)
, _state(STATE_UNLOADED)
{
}

bool File::Exists() const
{
	return true;
}

const String& File::GetFilename() const
{
	return _filename;
}

const String& File::GetFileExtension() const
{
	return str_Default;
}

const String& File::GetFilenameWithoutExtension() const
{
	return str_Default;
}

const String& File::GetAbsolutePath() const
{
	return str_Default;
}

uint16_t File::GetFileSize() const
{
	return 0;
}

size_t File::GetDataSize() const
{
	return _data.size();
}

bool File::HasData() const
{
	return _data.empty();
}

const DataBuffer& File::GetData() const
{
	return _data;
}

DataBuffer& File::GetData()
{
	return _data;
}

File::State File::GetState() const
{
	return _state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions used by FileIOMgr
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::SetState(State state)
{
	_state = state;
}

void File::ClearDataBuffer()
{
	if(_state == STATE_LOADED || _state == STATE_UNLOADED)
	{
		_data.clear();
		_state = STATE_UNLOADED;
	}
}

Result<void, Error> File::InvokeWriteCallback(const WriteResult& result)
{
	if(_state == STATE_WRITING_TO_DISK)
	{
		return FIRE_RESULT_OK;
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not invoke write callback. '" + _filename + "' is not queued for write");
}

Result<void, Error> File::InvokeReadCallback(const ReadResult& result)
{
	if(_state == STATE_READING_FROM_DISK)
	{
		return FIRE_RESULT_OK;
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not invoke read callback. '" + _filename + "' is not queued for read");
}

Result<void, Error> File::PerformDiskWriteSync()
{
	if(_state == STATE_QUEUED_FOR_WRITE)
	{
		_state = STATE_WRITING_TO_DISK;
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not write data to disk. '" + _filename + "' is not queued for write");
}

Result<void, Error> File::PerformDiskWriteAsync()
{
	if(_state == STATE_QUEUED_FOR_WRITE)
	{
		_state = STATE_WRITING_TO_DISK;
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not write data to disk. '" + _filename + "' is not queued for write");
}

Result<void, Error> File::PerformDiskReadSync()
{
	if(_state == STATE_QUEUED_FOR_READ)
	{
		_state = STATE_READING_FROM_DISK;
		PHYSFS_File* file = PHYSFS_openRead(_filename.c_str());
		if(file)
		{
			size_t len = PHYSFS_fileLength(file);
			_data.clear();
			_data.reserve(len);

			PHYSFS_readBytes(file, &_data[0], len);
			PHYSFS_close(file);
			_state = STATE_LOADED;

			return FIRE_RESULT_OK;
		}
		else
		{
			_state = STATE_UNLOADED;
			return FIRE_ERROR(ERROR_COULD_NOT_READ_DATA, String(PHYSFS_getLastError()));
		}
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not read data from disk. '" + _filename + "' is not queued for load");
}

Result<void, Error> File::PerformDiskReadAsync()
{
	if(_state == STATE_QUEUED_FOR_READ)
	{
		_state = STATE_READING_FROM_DISK;
	}
	return FIRE_ERROR(ERROR_IMPROPER_STATE, "could not read data from disk. '" + _filename + "' is not queued for load");
}


CLOSE_NAMESPACE(Firestorm);