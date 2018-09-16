#include "stdafx.h"
#include "libIO.h"

#include <physfs/physfs.h>
#include "Logger.h"
#include <libCore/ArgParser.h>

#include "IResourceObject.h"
#include "ResourceReference.h"

#include "FileIOMgr.h"

OPEN_NAMESPACE(Firestorm);

static void LogLastPhysfsError(const String& preamble)
{
	PHYSFS_ErrorCode lastErrorCode = PHYSFS_getLastErrorCode();
	if(lastErrorCode != PHYSFS_ERR_OK)
		FIRE_LOG_ERROR("%s -> %s", preamble, PHYSFS_getErrorByCode(lastErrorCode));
}

void libIO::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<ResourceReference>();
	Lib::RegisterReflection<IResourceObject>();
	Lib::RegisterReflection<FileLoadedEvent>();
	Lib::RegisterReflection<FileLoadErrorEvent>();

	ArgParser parser(ac, av);

	FIRE_ASSERT(parser.Has("--AppName") && "Application must have --AppName defined as an argument");

	String assetsDir(parser.Get("--AssetsDir", av[0]));

	if(PHYSFS_init(assetsDir.c_str()) != 0)
	{
		LogLastPhysfsError("Error Initializing physfs");
	}
	String appName(parser.Get("--AppName", String()));

	if(!PHYSFS_isDirectory(appName.c_str()))
	{
		LogLastPhysfsError("'" + appName + "' directory does not exist");
	}

	if(!libIO::Mount(assetsDir.c_str(),"/"))
		LogLastPhysfsError("Error when mounting 'Assets' directory");

	String appDir(assetsDir + "/" + appName.c_str());

	if(!libIO::Mount(appDir, "/"))
		LogLastPhysfsError("Error when mounting '"+appName+"' directory");

	String modules(parser.Get("--Modules", ""));
	if(!modules.empty())
	{
		Vector<String> mods = SplitString(modules, ',');
		for(auto mod : mods)
		{
			if(!libIO::Mount(assetsDir + "/" + mod, "/"))
				LogLastPhysfsError("Error when mounting module '" + mod + "'.");
		}
	}

	String prefDir(PHYSFS_getPrefDir("com.org.firestorm", appName.c_str()));
	PHYSFS_setWriteDir(prefDir.c_str());
}

bool libIO::Mount(const String& dir, const String& mountPoint)
{
	if(PHYSFS_mount(dir.c_str(), mountPoint.c_str(), true) != PHYSFS_ERR_OK)
	{
		return false;
	}
	return true;
}

Result<Vector<char>, Error> libIO::LoadFile(const String& filename)
{
	Vector<char> data;
	PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
	if (file)
	{
		PHYSFS_sint64 len = PHYSFS_fileLength(file);
		if(len == -1)
		{
			PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
			return FIRE_ERROR(err, PHYSFS_getErrorByCode(err));
		}
		data.reserve(static_cast<size_t>(len));
		data.resize(static_cast<size_t>(len));

		PHYSFS_sint64 read = PHYSFS_readBytes(file, &data[0], len);
		if(read == -1)
		{
			PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
			return FIRE_ERROR(err, PHYSFS_getErrorByCode(err));
		}
	}
	else
	{
		PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
		return FIRE_ERROR(err, PHYSFS_getErrorByCode(err));
	}
	PHYSFS_close(file);
	return data;
}

Result<String, Error> libIO::LoadFileString(const String& filename)
{
	Result<Vector<char>, Error> data = LoadFile(filename);
	if (data.has_value())
	{
		auto d = data.value();
		return FIRE_RESULT(String(d.begin(), d.end()));
	}
	Error e = data.error();
	return FIRE_ERROR(e.Code, e.Message);
}

CLOSE_NAMESPACE(Firestorm);