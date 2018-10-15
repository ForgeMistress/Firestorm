///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RenderMgr
//
//  The rendering system manager. Only one of these should exist per Application.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_RENDERMGR_H_
#define LIBSCENE_RENDERMGR_H_
#pragma once

#include "RenderSystem.h"

OPEN_NAMESPACE(Firestorm);

class ResourceMgr;
class ObjectMaker;

class RenderMgr final
{
public:
	RenderMgr(ResourceMgr& fileIOMgr, ObjectMaker& objectMaker);
	~RenderMgr();

	void Initialize(const char* renderingSystem);

	void Shutdown();

	const char* GetRenderer() const;
	const char* GetDevice() const;
	const char* GetVendor() const;
	const char* GetShadingLanguageName() const;

private:
	ResourceMgr& _fileIOMgr;
	ObjectMaker& _objectMaker;
	RenderSystem _system;

	string _rendererName;
};

CLOSE_NAMESPACE(Firestorm);

#endif