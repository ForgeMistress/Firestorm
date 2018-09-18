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

#include <libIO/FileIOMgr.h>
#include <libMirror/ObjectMaker.h>

OPEN_NAMESPACE(Firestorm);

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
	RenderMgr(FileIOMgr& fileIOMgr, ObjectMaker& objectMaker);
	~RenderMgr();

	void Initialize(const String& renderingSystem, const LLGL::RenderContextDescriptor& renderContextDesc);

	bool IsUsingRenderer(const String& api);

	const String& GetRenderer() const;
	const String& GetDevice() const;
	const String& GetVendor() const;
	const String& GetShadingLanguageName() const;

public:
	std::unique_ptr<LLGL::RenderSystem> System{ nullptr };
	LLGL::RenderContext* Context{ nullptr };

	FileIOMgr& _fileIOMgr;
	ObjectMaker& _objectMaker;
};

CLOSE_NAMESPACE(Firestorm);

#endif