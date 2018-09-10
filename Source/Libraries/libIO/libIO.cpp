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

static bool Mount(const String& dir, const String& mountPoint)
{
	if(PHYSFS_mount(dir.c_str(), mountPoint.c_str(), true) != PHYSFS_ERR_OK)
	{
		return false;
	}
	else
	{
		FIRE_LOG_DEBUG(":: Mounted Directory", dir, "to mount point", mountPoint);
	}
	return true;
}

CLOSE_NAMESPACE(Firestorm);

void Firestorm::libIO::Initialize(int ac, char** av)
{
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;
	ArgParser parser(ac, av);

	FIRE_ASSERT(parser.Has("--AppName") && "Application must have --AppName defined as an argument");

	String workingDir(parser.Get("--WorkingDir", av[0]));

	FIRE_LOG_DEBUG("Working Directory Is -> ", workingDir);
	if(PHYSFS_init(workingDir.c_str()) != 0)
	{
		LogLastPhysfsError("Error Initializing physfs");
	}
	String appName(parser.Get("--AppName", String()));

	if(!PHYSFS_isDirectory(appName.c_str()))
	{
		LogLastPhysfsError("'"+appName+"' directory does not exist");
	}

	if(!Mount(appName.c_str(), "/"))
		LogLastPhysfsError("Error when mounting 'Assets' directory");

	String prefDir(PHYSFS_getPrefDir("com.org.firestorm", appName.c_str()));
	PHYSFS_setWriteDir(prefDir.c_str());
	FIRE_LOG_DEBUG("Pref Dir:", prefDir);
}
