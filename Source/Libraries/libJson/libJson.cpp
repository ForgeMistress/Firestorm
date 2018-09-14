#include "stdafx.h"
#include "libJson.h"
#include "JSONDocument.h"

OPEN_NAMESPACE(Firestorm);

void libJson::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<JSONDocument>();
}

CLOSE_NAMESPACE(Firestorm);
