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
: _renderMgr(_resourceMgr, _objectMaker)
{
}

ManagerMgr::~ManagerMgr()
{
	_resourceMgr.Shutdown();
	_renderMgr.Shutdown();
}

void ManagerMgr::Shutdown()
{
	_resourceMgr.Shutdown();
	_renderMgr.Shutdown();
}

ResourceMgr& ManagerMgr::GetResourceMgr()
{
	return _resourceMgr;
}

RenderMgr& ManagerMgr::GetRenderMgr()
{
	return _renderMgr;
}

ObjectMaker& ManagerMgr::GetObjectMaker()
{
	return _objectMaker;
}

CLOSE_NAMESPACE(Firestorm);