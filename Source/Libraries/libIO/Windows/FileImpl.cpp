///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileImpl.cpp
//
//  The Windows implementation of the File type.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#ifdef FIRE_PLATFORM_WINDOWS
#include "../File.h"

OPEN_NAMESPACE(Firestorm);

struct ImplData
{
};

File::Impl::Impl()
: m_data(new ImplData())
{
}

Result<DataBuffer, Error> File::Impl::PerformDiskReadSync(const String& filename)
{
	return FIRE_RESULT(DataBuffer, DataBuffer());
}

Result<void, Error> File::Impl::PerformDiskWriteSync(const String& filename, const DataBuffer& data)
{
	return FIRE_RESULT(void);
}

CLOSE_NAMESPACE(Firestorm);

#endif