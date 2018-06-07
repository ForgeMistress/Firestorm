
#include "stdafx.h"
#include "../File.h"

OPEN_NAMESPACE(Elf);

struct File::FileImpl
{
	FileImpl(const string& filename)
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

	string filename;
};

File::File(const string& filename)
	: m_impl(new FileImpl(filename))
{
}

bool File::Exists() const
{
	return true;
}

const string& File::GetFilename() const
{
	return str_Default;
}

const string& File::GetFileExtension() const
{
	return str_Default;
}
const string& File::GetPath() const
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

ResultCode File::WriteToDiskSync() const
{
	return Result::OK;
}

ResultCode File::WriteToDiskAsync(const AsyncCallback& callback) const
{
	return Result::OK;
}

ResultCode File::ReadFromDiskSync()
{
	return Result::OK;
}

ResultCode File::ReadFromDiskAsync(const AsyncCallback& callback)
{
	return Result::OK;
}

File::State File::GetState() const
{
	return UNLOADED;
}

CLOSE_NAMESPACE(Elf);

#endif