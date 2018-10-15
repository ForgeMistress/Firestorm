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

OPEN_NAMESPACE(Firestorm);

ManagerMgr::ManagerMgr()
: _FIRE_MGR_VAR(RenderMgr)(_FIRE_MGR_VAR(ResourceMgr), _FIRE_MGR_VAR(ObjectMaker))
, _FIRE_MGR_VAR(EntityMgr)(_FIRE_MGR_VAR(UUIDMgr))
{
}

ManagerMgr::~ManagerMgr()
{
	Shutdown();
}

void ManagerMgr::Initialize()
{
	_FIRE_MGR_VAR(RenderMgr).Initialize("");
}

void ManagerMgr::Shutdown()
{
	_FIRE_MGR_VAR(ResourceMgr).Shutdown();
	_FIRE_MGR_VAR(RenderMgr).Shutdown();
}

CLOSE_NAMESPACE(Firestorm);