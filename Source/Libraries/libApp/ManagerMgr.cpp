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
#include <thread>
#include <libCore/TaskGraph.h>

OPEN_NAMESPACE(Firestorm);

ManagerMgr::ManagerMgr(Application& app)
: _app(app)
, _Window(_app)
, _ResourceMgr(_app)
, _RenderMgr(_app)
, _EntityMgr(_app)
, _TaskGraph(_app)
{
}

ManagerMgr::~ManagerMgr()
{
	Shutdown();
}

void ManagerMgr::Initialize()
{
	_Window.Initialize(WindowDesc{
		"Firestorm Application",
		800,
		600
	});
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	_RenderMgr.Initialize("");
}

void ManagerMgr::UpdateSystems(double deltaT)
{
	_TaskGraph.wait_for_all();
	_TaskGraph.clear();

	_ResourceMgr.CleanOldResources();
}

void ManagerMgr::Shutdown()
{
	FIRE_LOG_DEBUG("!! Shutting down task graph...");
	_TaskGraph.Shutdown();

	FIRE_LOG_DEBUG("!! Shutting down RenderMgr");
	_RenderMgr.Shutdown();
}

class Window& ManagerMgr::GetWindow()
{
	return _Window;
}

class UUIDMgr& ManagerMgr::GetUUIDMgr()
{
	return _UUIDMgr;
}

class ResourceMgr& ManagerMgr::GetResourceMgr()
{
	return _ResourceMgr;
}

class RenderMgr& ManagerMgr::GetRenderMgr()
{
	return _RenderMgr;
}

class ObjectMaker& ManagerMgr::GetObjectMaker()
{
	return _ObjectMaker;
}

class EntityMgr& ManagerMgr::GetEntityMgr()
{
	return _EntityMgr;
}

class TaskGraph& ManagerMgr::GetTaskGraph()
{
	return _TaskGraph;
}

CLOSE_NAMESPACE(Firestorm);