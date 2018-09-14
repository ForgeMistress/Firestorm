#include "stdafx.h"
#include "libScript.h"

#include "ScriptComponent.h"

OPEN_NAMESPACE(Firestorm);
void libScript::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<ScriptComponent>();
}
CLOSE_NAMESPACE(Firestorm);
