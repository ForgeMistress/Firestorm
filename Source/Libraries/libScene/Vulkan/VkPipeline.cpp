///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkPipeline
//
//  Implementation of the RenderSystem class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkPipeline.h"

OPEN_NAMESPACE(Firestorm);


Vk_PipelineLayout::Vk_PipelineLayout(RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

Vk_PipelineLayout::~Vk_PipelineLayout()
{
	//_renderSystem.Release(this);
}



Vk_Pipeline::CreateInfo::CreateInfo(class RenderSystem& renderSystem)
{
	const auto& swapchainExtent = renderSystem.GetSwapchainExtent();
	Viewports.push_back({
		narrow_cast<float>(swapchainExtent.width),
		narrow_cast<float>(swapchainExtent.height),
		narrow_cast<float>(swapchainExtent.width),
		narrow_cast<float>(swapchainExtent.height),
		0.0f,
		1.0f
	});

	Scissors.push_back({
		0,
		0,
		swapchainExtent.width,
		swapchainExtent.height
	});

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
}

Vk_Pipeline::Vk_Pipeline(class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

Vk_Pipeline::~Vk_Pipeline()
{
}

CLOSE_NAMESPACE(Firestorm);