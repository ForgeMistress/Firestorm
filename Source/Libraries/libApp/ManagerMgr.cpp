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

#include <libIO/ResourceMgr.h>
#include <libScene/RenderMgr.h>
#include <libMirror/ObjectMaker.h>

OPEN_NAMESPACE(Firestorm);

ManagerMgr::ManagerMgr()
: _resourceMgr(new ResourceMgr())
, _objectMaker(new ObjectMaker())
, _renderMgr(new RenderMgr(*_resourceMgr, *_objectMaker))
{
}

ManagerMgr::~ManagerMgr()
{
	_resourceMgr->Shutdown();
	_renderMgr->Shutdown();

	delete _resourceMgr;
	delete _objectMaker;
	delete _renderMgr;
}

void ManagerMgr::Shutdown()
{
	_resourceMgr->Shutdown();
	_renderMgr->Shutdown();
}

ResourceMgr& ManagerMgr::GetResourceMgr()
{
	return *_resourceMgr;
}

RenderMgr& ManagerMgr::GetRenderMgr()
{
	return *_renderMgr;
}

ObjectMaker& ManagerMgr::GetObjectMaker()
{
	return *_objectMaker;
}

CLOSE_NAMESPACE(Firestorm);