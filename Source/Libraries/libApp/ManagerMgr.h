///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ManagerMgr
//
//  Manages all of the systems in one place. Whenever a new system is needed at a global scope, add it here.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_MANAGERMGR_H_
#define LIBAPP_MANAGERMGR_H_
#pragma once

#include "Window.h"
#include <libCore/ObjectMaker.h>
#include <libCore/UUIDMgr.h>
#include <libIO/ResourceMgr.h>
#include <libScene/RenderMgr.h>
#include <libExistence/Entity.h>
#include <libCore/TaskGraph.h>

OPEN_NAMESPACE(Firestorm);

#define _FIRE_MGR_VAR(TYPE) _##TYPE

#define FIRE_MGR_INSTALL(TYPE)      \
private:                            \
	class TYPE _FIRE_MGR_VAR(TYPE); \
public:                             \
	class TYPE& Get##TYPE();

class ManagerMgr final
{
private:
	friend class Application;
	ManagerMgr(Application& app);
	~ManagerMgr();

public:
	void Initialize();
	void UpdateSystems(double deltaT);
	void Shutdown();

	class Window&       GetWindow();
	class UUIDMgr&      GetUUIDMgr();
	class ResourceMgr&  GetResourceMgr();
	class RenderMgr&    GetRenderMgr();
	class ObjectMaker&  GetObjectMaker();
	class EntityMgr&    GetEntityMgr();
	class TaskGraph&    GetTaskGraph();

private:
	class Application& _app;
	Window       _Window;
	UUIDMgr      _UUIDMgr;
	ResourceMgr  _ResourceMgr;
	RenderMgr    _RenderMgr;
	ObjectMaker  _ObjectMaker;
	EntityMgr    _EntityMgr;
	TaskGraph    _TaskGraph;
};


CLOSE_NAMESPACE(Firestorm);

#endif