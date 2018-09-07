#include "stdafx.h"
#include "libMirror.h"
#include "Object.h"

void Firestorm::libMirror::Initialize(int ac, char** av)
{
	FIRE_MIRROR_REGISTER(Firestorm::Mirror::Object);
}
