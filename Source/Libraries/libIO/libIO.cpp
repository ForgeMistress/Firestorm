#include "stdafx.h"
#include "libIO.h"

#include <physfs/physfs.h>
#include "Logger.h"
#include <libCore/ArgParser.h>

OPEN_NAMESPACE(Firestorm);

static void LogLastPhysfsError(const String& preamble)
{
	PHYSFS_ErrorCode lastErrorCode = PHYSFS_getLastErrorCode();
	if(lastErrorCode != PHYSFS_ERR_OK)
		FIRE_LOG_ERROR(preamble, "->", String(PHYSFS_getErrorByCode(lastErrorCode)));
}

void libIO::Initialize(int ac, char** av)
{
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;

	ArgParser parser(ac, av);

	FIRE_ASSERT(parser.Has("--AppName") && "Application must have --AppName defined as an argument");

	String assetsDir(parser.Get("--AssetsDir", av[0]));

	FIRE_LOG_DEBUG(":: Working Directory Is -> ", assetsDir);
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
	if(PHYSFS_isDirectory(appDir.c_str()) || PHYSFS_exists(appDir.c_str()))
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

	FIRE_LOG_DEBUG(":: Preferences directory is set to:", prefDir);
}

bool libIO::Mount(const String& dir, const String& mountPoint)
{
	if(PHYSFS_mount(dir.c_str(), mountPoint.c_str(), true) != PHYSFS_ERR_OK)
	{
		return false;
	}
	FIRE_LOG_DEBUG(":: Mounted Directory", dir, "to mount point", mountPoint);
	return true;
}


Vector<char> libIO::LoadFile(const String& filename)
{
	Vector<char> data;
	PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
	if(file)
	{
		PHYSFS_sint64 len = PHYSFS_fileLength(file);
		data.reserve(len);
		data.resize(len);

		PHYSFS_readBytes(file, &data[0], len);
		PHYSFS_close(file);
	}
	return data;
}

String libIO::LoadFileString(const String& filename)
{
	Vector<char> data = LoadFile(filename);
	return String(data.begin(), data.end());
}

CLOSE_NAMESPACE(Firestorm);