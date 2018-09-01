#include "stdafx.h"
#include "libMirror.h"
#include "Object.h"

void Elf::libMirror::Initialize()
{
	ELF_MIRROR_REGISTER(Elf::Mirror::Object);
}
