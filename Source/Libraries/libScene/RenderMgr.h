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
class Application;

class RenderMgr final
{
public:
	RenderMgr(Application& app);
	~RenderMgr();

	void Initialize(const char* renderingSystem);

	void Shutdown();

	const char* GetRenderer() const;
	const char* GetDevice() const;
	const char* GetVendor() const;
	const char* GetShadingLanguageName() const;

	Application& GetApp() const { return _app; }

	RenderSystem& GetSystem() { return _system; }
	const RenderSystem& GetSystem() const { return _system; }

private:
	Application& _app;
	RenderSystem _system;

	string _rendererName;
};

CLOSE_NAMESPACE(Firestorm);

#endif