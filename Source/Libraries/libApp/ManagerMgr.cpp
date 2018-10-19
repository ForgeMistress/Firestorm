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

OPEN_NAMESPACE(Firestorm);

ManagerMgr::ManagerMgr(Application& app)
: _app(app)
, _Window(_app)
, _ResourceMgr(_app)
, _RenderMgr(_app)
, _EntityMgr(_app)
, _Taskflow(std::thread::hardware_concurrency())
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
	FIRE_LOG_DEBUG("!! Shutting down task graph...");
	_Taskflow.wait_for_all();

	FIRE_LOG_DEBUG("!! Shutting down RenderMgr");
	_RenderMgr.Shutdown();
}

class Window& ManagerMgr::Window()
{
	return _Window;
}

class UUIDMgr& ManagerMgr::UUIDMgr()
{
	return _UUIDMgr;
}

class ResourceMgr& ManagerMgr::ResourceMgr()
{
	return _ResourceMgr;
}

class RenderMgr& ManagerMgr::RenderMgr()
{
	return _RenderMgr;
}

class ObjectMaker& ManagerMgr::ObjectMaker()
{
	return _ObjectMaker;
}

class EntityMgr& ManagerMgr::EntityMgr()
{
	return _EntityMgr;
}

class tf::Taskflow& ManagerMgr::Taskflow()
{
	return _Taskflow;
}

CLOSE_NAMESPACE(Firestorm);