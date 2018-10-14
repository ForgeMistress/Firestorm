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
#include <libCore/UUIDMgr.h>
#include <libIO/ResourceMgr.h>
#include <libScene/RenderMgr.h>
#include <libMirror/ObjectMaker.h>
#include <libExistence/Entity.h>

OPEN_NAMESPACE(Firestorm);

#define _FIRE_MGR_VAR(TYPE) _##TYPE##_INSTANCE

#define FIRE_MGR_INSTALL(TYPE)\
private:\
	TYPE _FIRE_MGR_VAR(TYPE);\
public:\
	TYPE& Get##TYPE () { return _FIRE_MGR_VAR(TYPE); }

class ManagerMgr final
{
private:
	friend class Application;
	ManagerMgr();
	~ManagerMgr();

public:
	void Shutdown();

	FIRE_MGR_INSTALL(UUIDMgr);
	FIRE_MGR_INSTALL(ResourceMgr);
	FIRE_MGR_INSTALL(RenderMgr);
	FIRE_MGR_INSTALL(ObjectMaker);
	FIRE_MGR_INSTALL(EntityMgr);
};


CLOSE_NAMESPACE(Firestorm);

#endif