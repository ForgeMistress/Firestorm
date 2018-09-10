#include "stdafx.h"
#include "libIO.h"

#include <physfs/physfs.h>
#include "Logger.h"

void Firestorm::libIO::Initialize(int ac, char** av)
{
	PHYSFS_init(av[0]);
	PHYSFS_permitSymbolicLinks(1);

	PHYSFS_mount("Assets", "/", true);

	String baseDir = PHYSFS_getBaseDir();
	FIRE_LOG_DEBUG(baseDir);

	PHYSFS_mount(baseDir.c_str(), "/", true);

	String prefDir(PHYSFS_getPrefDir("com.firestorm","Firestorm"));
	FIRE_LOG_DEBUG("Pref Dir:", prefDir);
}
