///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkPipeline
//
//  Implementation of the RenderSystem class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKPIPELINE_H_
#define LIBSCENE_VKPIPELINE_H_
#pragma once

#include "../RenderMgr.h"
#include "../PrimitiveTopology.h"

#include "../IPipeline.h"

#include "VkShaderProgram.h"
#include <libMath/Vector.h>

OPEN_NAMESPACE(Firestorm);

class Vk_PipelineLayout final : public IPipelineLayout
{
public:
	Vk_PipelineLayout(class RenderSystem& renderSystem);

	virtual ~Vk_PipelineLayout();

private:
	friend class RenderSystem;
	class RenderSystem& _renderSystem;

	VkPipelineLayout _vkLayout;
};

class Vk_Pipeline final : public IPipeline
{
public:
	Vk_Pipeline(class RenderSystem& renderSystem);
	virtual ~Vk_Pipeline();

private:
	friend class RenderSystem;
	class RenderSystem& _renderSystem;

	VkPipeline _pipeline;
};

CLOSE_NAMESPACE(Firestorm);

#endif