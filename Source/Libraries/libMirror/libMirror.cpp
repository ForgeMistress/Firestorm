#include "stdafx.h"
#include "libMirror.h"
#include "Object.h"

OPEN_NAMESPACE(Firestorm);

void libMirror::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<Mirror::Object>();
}

CLOSE_NAMESPACE(Firestorm);
