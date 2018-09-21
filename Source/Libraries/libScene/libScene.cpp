#include "stdafx.h"
#include "libScene.h"

#include "MeshResource.h"
#include "ShaderResource.h"

OPEN_NAMESPACE(Firestorm);

void libScene::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<MeshResource>();
	Lib::RegisterReflection<ShaderResource>();
}

CLOSE_NAMESPACE(Firestorm);
