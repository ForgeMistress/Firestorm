#include "stdafx.h"
#include "libIO.h"

#include <physfs/physfs.h>

void Firestorm::libIO::Initialize(int ac, char** av)
{
	PHYSFS_init(av[0]);
}
