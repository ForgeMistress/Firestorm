
#include "stdafx.h"

#ifdef ELF_PLATFORM_WINDOWS
#include "../File.h"

OPEN_NAMESPACE(Elf);

struct File::FileImpl
{
	FileImpl(const String& filename)
	: filename(filename)
	{
	}

	/*const string& GetFilename() const;
	const string& GetFileExtension() const;
	const string& GetPath() const;
	uint16_t GetFileSize() const;
	size_t GetDataSize() const;
	const char* GetData() const;
	ResultCode WriteToDiskSync() const;
	ResultCode WriteToDiskAsync(const AsyncCallback& callback) const;
	ResultCode ReadFromDiskSync();
	ResultCode ReadFromDiskAsync(const AsyncCallback& callback);
	State GetState() const;*/

	String filename;
};

File::File(const String& filename)
	: m_impl(new FileImpl(filename))
{
}

bool File::Exists() const
{
	return true;
}

const String& File::GetFilename() const
{
	return str_Default;
}

const String& File::GetFileExtension() const
{
	return str_Default;
}

const String& File::GetFilenameWithoutExtension() const
{
	return str_Default;
}

const String& File::GetPath() const
{
	return str_Default;
}

uint16_t File::GetFileSize() const
{
	return 0;
}

size_t File::GetDataSize() const
{
	return 0;
}

const char* File::GetData() const
{
	return nullptr;
}

Result<void> File::WriteToDiskSync() const
{
	return result();
}

Result<void> File::WriteToDiskAsync(const AsyncCallback& callback) const
{
	return result();
}

Result<void> File::ReadFromDiskSync()
{
	return result();
}

Result<void> File::ReadFromDiskAsync(const AsyncCallback& callback)
{
	return result();
}

File::State File::GetState() const
{
	return UNLOADED;
}

CLOSE_NAMESPACE(Elf);

#endif