#include "stdafx.h"
#include "libIO.h"

#include <physfs/physfs.h>

#include <libCore/Logger.h>
#include <libCore/ArgParser.h>

#include "IResourceObject.h"
#include "ResourceReference.h"

#include "ResourceMgr.h"

OPEN_NAMESPACE(Firestorm);

const ErrorCode* libIO::INTERNAL_ERROR(new ErrorCode("there was an error that occurred with the internal libraries"));

static void LogLastPhysfsError(const string& preamble)
{
	PHYSFS_ErrorCode lastErrorCode = PHYSFS_getLastErrorCode();
	if(lastErrorCode != PHYSFS_ERR_OK)
	{
		const char* err = PHYSFS_getErrorByCode(lastErrorCode);
		FIRE_LOG_ERROR("%s -> %s", preamble.c_str(), err);
	}
}

void libIO::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<ResourceReference>();

	ArgParser parser(ac, av);

	FIRE_ASSERT(parser.Has("--AppName") && "Application must have --AppName defined as an argument");

	string assetsDir(parser.Get("--AssetsDir", av[0]));

	FIRE_LOG_DEBUG(":: Initializing search path to %s", assetsDir.c_str());
	if(PHYSFS_init(av[0]?av[0]:parser.Get("--AppName","").c_str()) != 0)
	{
		LogLastPhysfsError("Error Initializing physfs");
	}
	string appName(parser.Get("--AppName", ""));

	if(!PHYSFS_isDirectory(appName.c_str()))
	{
		LogLastPhysfsError("'" + appName + "' directory does not exist");
	}

	bool assetsMountResult = libIO::Mount(assetsDir, "/");
	FIRE_ASSERT_MSG(assetsMountResult, "Mounting assets directory failed.");

	string appDir(assetsDir + "/" + appName.c_str());

	bool appMountResult = libIO::Mount(appDir, "/");
	FIRE_ASSERT_MSG(appMountResult, Format("Failure mounting app assets directory %s", appDir.c_str()).c_str());

	string modules(parser.Get("--Modules", ""));
	if(!modules.empty())
	{
		vector<string> mods = SplitString(modules, ',');
		for(auto mod : mods)
		{
			bool result = libIO::Mount(assetsDir + "/" + mod, "/");
			FIRE_ASSERT_MSG(result, Format("Mounting module %s failed.", mod.c_str()));
		}
	}

	string prefDir(PHYSFS_getPrefDir("com.org.firestorm", appName.c_str()));
	PHYSFS_setWriteDir(prefDir.c_str());
}

bool libIO::Mount(const string& dir, const string& mountPoint)
{
	FIRE_LOG_DEBUG(":: Mounting directory %s to mount point %s", dir.c_str(), mountPoint.c_str());
	if(PHYSFS_mount(dir.c_str(), mountPoint.c_str(), true) == 0)
	{
		LogLastPhysfsError(Format("Error mounting directory %s", dir.c_str()));
		return false;
	}
	return true;
}

bool libIO::FileExists(const char* filename)
{
	FIRE_LOG_DEBUG("Checking for file %s", filename);
	if(PHYSFS_exists(filename) == 0)
	{
		LogLastPhysfsError(Format("Error finding file %s", filename));
		return false;
	}
	return true;
}

Result<vector<char>, Error> libIO::LoadFile(const string& filename)
{
	vector<char> data;
	PHYSFS_File* file = PHYSFS_openRead(filename.c_str());
	if(file)
	{
		PHYSFS_sint64 len = PHYSFS_fileLength(file);
		if(len == -1)
		{
			PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
			return FIRE_ERROR(INTERNAL_ERROR, PHYSFS_getErrorByCode(err));
		}
		data.reserve(static_cast<size_t>(len));
		data.resize(static_cast<size_t>(len));

		PHYSFS_sint64 read = PHYSFS_readBytes(file, &data[0], len);
		if(read == -1)
		{
			PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
			return FIRE_ERROR(INTERNAL_ERROR, PHYSFS_getErrorByCode(err));
		}
	}
	else
	{
		PHYSFS_ErrorCode err = PHYSFS_getLastErrorCode();
		return FIRE_ERROR(INTERNAL_ERROR, PHYSFS_getErrorByCode(err));
	}
	PHYSFS_close(file);
	return data;
}

Result<string, Error> libIO::LoadFileString(const string& filename)
{
	Result<vector<char>, Error> data = LoadFile(filename);
	if(data.has_value())
	{
		auto d = data.value();
		return FIRE_RESULT(string(d.begin(), d.end()));
	}
	return FIRE_FORWARD_ERROR(data.error());
}

static PHYSFS_EnumerateCallbackResult enumerateGetFiles(void* data, const char* origData, const char* fname)
{
	vector<string>* ptr = static_cast<vector<string>*>(data);
	ptr->push_back(fname);
	return PHYSFS_ENUM_OK;
}

vector<string> libIO::GetFiles(const string& path)
{
	/*
	PHYSFS_EnumerateCallbackResult (*PHYSFS_EnumerateCallback)(void *data,
									   const char *origdir, const char *fname);
	*/
	vector<string> outFiles;
	PHYSFS_enumerate(path.c_str(), enumerateGetFiles, &outFiles);
	return outFiles;
}

CLOSE_NAMESPACE(Firestorm);