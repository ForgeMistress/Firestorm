///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceMgr.h"

#include "ResourceReference.h"
#include "ResourceIOErrors.h"

#include <libApp/Application.h>

#include <sstream>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ResourceMgr::RootTask = "ResourceMgr::RootTask";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceMgr::ResourceMgr(Application& app)
: _app(app)
, _tg(_app.GetSystems().GetTaskGraph())
{
}

ResMgrProxy::ResMgrProxy(Application& app, ResourceMgr& mgr, tf::SubflowBuilder& builder, const char* filename)
: _app(app)
, _mgr(mgr)
, _tf(_app.GetSystems().GetTaskGraph())
, _builder(builder)
, _thisFilename(filename)
{
}

CLOSE_NAMESPACE(Firestorm);