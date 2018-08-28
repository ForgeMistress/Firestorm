
#include "stdafx.h"
#include "File.h"

OPEN_NAMESPACE(Elf);

File::File(const FileIOMgr* fileIOMgr, const String& filename)
: m_mgr(fileIOMgr)
, m_impl(new Impl())
, m_state(STATE_UNLOADED)
{
}

bool File::Exists() const
{
	return true;
}

const String& File::GetFilename() const
{
	return m_filename;
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
	return m_data.size();
}

bool File::HasData() const
{
	return m_data.empty();
}

const DataBuffer& File::GetData() const
{
	return m_data;
}

DataBuffer& File::GetData()
{
	return m_data;
}

File::State File::GetState() const
{
	return m_state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private functions used by FileIOMgr
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void File::SetState(State state)
{
	m_state = state;
}

void File::ClearDataBuffer()
{
	if(m_state == STATE_LOADED || m_state == STATE_UNLOADED)
	{
		m_data.clear();
		m_state = STATE_UNLOADED;
	}
}

Result<void, Error> File::InvokeWriteCallback(const WriteResult& result)
{
	if(m_state == STATE_WRITING_TO_DISK)
	{
		return ELF_RESULT(void);
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not invoke write callback. '" + m_filename + "' is not queued for write");
}

Result<void, Error> File::InvokeReadCallback(const ReadResult& result)
{
	if(m_state == STATE_READING_FROM_DISK)
	{
		return ELF_RESULT(void);
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not invoke read callback. '" + m_filename + "' is not queued for read");
}

Result<void, Error> File::PerformDiskWriteSync()
{
	if(m_state == STATE_QUEUED_FOR_WRITE)
	{
		m_state = STATE_WRITING_TO_DISK;
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not write data to disk. '" + m_filename + "' is not queued for write");
}

Result<void, Error> File::PerformDiskWriteAsync()
{
	if(m_state == STATE_QUEUED_FOR_WRITE)
	{
		m_state = STATE_WRITING_TO_DISK;
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not write data to disk. '" + m_filename + "' is not queued for write");
}

Result<void, Error> File::PerformDiskReadSync()
{
	if(m_state == STATE_QUEUED_FOR_READ)
	{
		m_state = STATE_READING_FROM_DISK;
		Result<DataBuffer, Error> result = m_impl->PerformDiskReadSync(m_filename);
		if(result.has_value())
		{
			m_data = std::move(result.value());
			m_state = STATE_LOADED;
			return ELF_RESULT(void);
		}
		else
		{
			m_state = STATE_UNLOADED;
			return ELF_ERROR(ERROR_COULD_NOT_READ_DATA, "error reading '" + m_filename + "' from disk : " + result.error().Message);
		}
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not read data from disk. '" + m_filename + "' is not queued for load");
}

Result<void, Error> File::PerformDiskReadAsync()
{
	if(m_state == STATE_QUEUED_FOR_READ)
	{
		m_state = STATE_READING_FROM_DISK;
	}
	return ELF_ERROR(ERROR_IMPROPER_STATE, "could not read data from disk. '" + m_filename + "' is not queued for load");
}


CLOSE_NAMESPACE(Elf);