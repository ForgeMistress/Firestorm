///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ManagerMgr
//
//  Manages all of the systems in one place. Whenever a new system is needed at a global scope, add it here.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_MANAGERMGR_H_
#define LIBAPP_MANAGERMGR_H_
#pragma once

#include "ObjectMaker.h"

OPEN_NAMESPACE(Firestorm);

class ResourceMgr;
class RenderMgr;
class ObjectMaker;

class ManagerMgr final
{
private:
	friend class Application;
	ManagerMgr();
	~ManagerMgr();

	void Shutdown();

public:
	ResourceMgr& GetResourceMgr();
	RenderMgr&   GetRenderMgr();
	ObjectMaker& GetObjectMaker();

private:
	ResourceMgr* _resourceMgr;
	RenderMgr*   _renderMgr;
	ObjectMaker* _objectMaker;
};


CLOSE_NAMESPACE(Firestorm);

#endif