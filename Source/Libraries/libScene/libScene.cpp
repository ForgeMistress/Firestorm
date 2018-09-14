#include "stdafx.h"
#include "libScene.h"

#include "MeshResource.h"

OPEN_NAMESPACE(Firestorm);

void libScene::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<MeshResource>();
}

CLOSE_NAMESPACE(Firestorm);
