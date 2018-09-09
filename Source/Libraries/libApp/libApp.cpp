#include "stdafx.h"
#include "libApp.h"
#include "Application.h"
#include "InputEvents.h"

void Firestorm::libApp::Initialize(int ac, char** av)
{
	FIRE_MIRROR_REGISTER(ApplicationWantsToCloseEvent);
	FIRE_MIRROR_REGISTER(MouseButtonEvent);
	FIRE_MIRROR_REGISTER(MouseMoveEvent);
}