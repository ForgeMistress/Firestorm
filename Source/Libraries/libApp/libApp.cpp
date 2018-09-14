#include "stdafx.h"
#include "libApp.h"
#include "Application.h"
#include "InputEvents.h"

OPEN_NAMESPACE(Firestorm);

void libApp::Initialize(int ac, char** av)
{
	Lib::RegisterReflection<ApplicationWantsToCloseEvent>();
	Lib::RegisterReflection<MouseButtonEvent>();
	Lib::RegisterReflection<MouseMoveEvent>();
}

CLOSE_NAMESPACE(Firestorm);