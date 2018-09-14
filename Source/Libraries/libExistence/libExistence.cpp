#include "stdafx.h"
#include "libExistence.h"

#include "Component.h"
#include "Engine.h"
#include "Entity.h"
#include "System.h"

void Firestorm::libExistence::Initialize(int ac, char** av)
{
	FIRE_MIRROR_REGISTER(Component);
	FIRE_MIRROR_REGISTER(Engine);
	FIRE_MIRROR_REGISTER(Entity);
	FIRE_MIRROR_REGISTER(System);
}
