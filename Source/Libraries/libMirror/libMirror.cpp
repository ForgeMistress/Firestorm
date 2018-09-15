#include "stdafx.h"
#include "libMirror.h"
#include "Object.h"

#include "IInspectableObject.h"

OPEN_NAMESPACE(Firestorm);

void libMirror::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<Mirror::Object>();
	Lib::RegisterReflection<Mirror::IInspectableObject>();
}

CLOSE_NAMESPACE(Firestorm);
