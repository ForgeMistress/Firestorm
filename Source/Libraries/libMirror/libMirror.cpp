#include "stdafx.h"
#include "libMirror.h"
#include "Object.h"

void Elf::libMirror::Initialize(int ac, char** av)
{
	ELF_MIRROR_REGISTER(Elf::Mirror::Object);
}
