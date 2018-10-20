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

#include "Window.h"
#include <libMirror/ObjectMaker.h>
#include <libCore/UUIDMgr.h>
#include <libIO/ResourceMgr.h>
#include <libScene/RenderMgr.h>
#include <libMirror/ObjectMaker.h>
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
	void Shutdown();

	class Window&       Window();
	class UUIDMgr&      UUIDMgr();
	class ResourceMgr&  ResourceMgr();
	class RenderMgr&    RenderMgr();
	class ObjectMaker&  ObjectMaker();
	class EntityMgr&    EntityMgr();
	class TaskGraph&    TaskGraph();

private:
	class Application& _app;
	class Window       _Window;
	class UUIDMgr      _UUIDMgr;
	class ResourceMgr  _ResourceMgr;
	class RenderMgr    _RenderMgr;
	class ObjectMaker  _ObjectMaker;
	class EntityMgr    _EntityMgr;
	class TaskGraph    _TaskGraph;
};


CLOSE_NAMESPACE(Firestorm);

#endif