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

#include <LLGL/LLGL.h>
#include <LLGL/RenderContext.h>

OPEN_NAMESPACE(Firestorm);

class ResourceMgr;
class ObjectMaker;

/**
	Struct containing identifier strings for different rendering backends
	that can be returned by RenderMgr::GetRenderer.
 **/
struct Renderers
{
	static constexpr char* OpenGL   = "OpenGL";
	static constexpr char* Direct3D = "Direct3D";
};

class RenderMgr final
{
public:
	RenderMgr(ResourceMgr& fileIOMgr, ObjectMaker& objectMaker);
	~RenderMgr();

	void Initialize(const string& renderingSystem, const LLGL::RenderContextDescriptor& renderContextDesc);

	void Shutdown();

	bool IsUsingRenderer(const string& api);

	string GetRenderer() const;
	string GetDevice() const;
	string GetVendor() const;
	string GetShadingLanguageName() const;

	LLGL::Buffer* CreateBuffer(const LLGL::BufferDescriptor& desc);

public:
	std::unique_ptr<LLGL::RenderSystem> System{ nullptr };
	LLGL::RenderContext* Context{ nullptr };

private:
	string _rendererName;

	ResourceMgr& _fileIOMgr;
	ObjectMaker& _objectMaker;
};

CLOSE_NAMESPACE(Firestorm);

#endif