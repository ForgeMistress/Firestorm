///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RenderMgr
//
//  The rendering system manager. Only one of these should exist per Application.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
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
	struct PipelineHolder
	{
		RSPtr<IPipeline> Default;
	};
	PipelineHolder Pipelines;

	struct PipelineLayoutHolder
	{
		RSPtr<IPipelineLayout> Default;
	};
	PipelineLayoutHolder PipelineLayouts;

	struct FramebufferHolder
	{

	};
	FramebufferHolder Framebuffers;
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

	//IPipelineLayout::CreateInfo GetPipelineLayoutInfo() const;
	//IRenderPass::CreateInfo GetRenderPassInfo() const;
	//IPipeline::CreateInfo GetPipelineCreateInfo() const;
	//IPipeline::CreateInfo GetPipelineCreateInfo(Resource<IShaderProgram> shaderStage,
    //                                            IPipelineLayout*         layout,
    //                                            IRenderPass*             renderPass) const;

private:
	void CreateDefaultPipelines();
	void CreateDefaultFramebuffers();

	Application& _app;
	RenderSystem _system;

	string _rendererName;
};

CLOSE_NAMESPACE(Firestorm);

#endif