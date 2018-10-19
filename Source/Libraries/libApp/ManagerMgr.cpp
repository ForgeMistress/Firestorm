///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ManagerMgr
//
//  Manages all of the systems in one place. Whenever a new system is needed at a global scope, add it here.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ManagerMgr.h"

#include "Application.h"

#define FIRE_MGR_IMPL(TYPE) class TYPE& ManagerMgr::Get##TYPE () { return _FIRE_MGR_VAR(TYPE); }

OPEN_NAMESPACE(Firestorm);

ManagerMgr::ManagerMgr(Application& app)
: _app(app)
, _FIRE_MGR_VAR(Window)(_app)
, _FIRE_MGR_VAR(RenderMgr)(_app)
, _FIRE_MGR_VAR(EntityMgr)(_app)
, _FIRE_MGR_VAR(TaskGraph)(_app)
{
}

ManagerMgr::~ManagerMgr()
{
	Shutdown();
}

void ManagerMgr::Initialize()
{
	_FIRE_MGR_VAR(Window).Initialize(WindowDesc{
		"Firestorm Application",
		800,
		600
	});
	_FIRE_MGR_VAR(RenderMgr).Initialize("");
}

void ManagerMgr::Shutdown()
{
	_FIRE_MGR_VAR(TaskGraph).Shutdown();
	_FIRE_MGR_VAR(ResourceMgr).Shutdown();
	_FIRE_MGR_VAR(RenderMgr).Shutdown();
}

FIRE_MGR_IMPL(Window);
FIRE_MGR_IMPL(UUIDMgr);
FIRE_MGR_IMPL(ResourceMgr);
FIRE_MGR_IMPL(RenderMgr);
FIRE_MGR_IMPL(ObjectMaker);
FIRE_MGR_IMPL(EntityMgr);
FIRE_MGR_IMPL(TaskGraph);

CLOSE_NAMESPACE(Firestorm);