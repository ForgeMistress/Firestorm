///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkRenderPass
//
//  Implementation of the RenderPass class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkRenderPass.h"

OPEN_NAMESPACE(Firestorm);

IRenderPass::CreateInfo::CreateInfo(const class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{

}

void IRenderPass::CreateInfo::AddAttachmentDesc(const IRenderPass::CreateInfo::AttachmentDescription& attachmentDesc)
{
	Attachments.push_back(attachmentDesc);
}

void IRenderPass::CreateInfo::AddSubpass(const SubpassDescription& subpass)
{
	Subpasses.push_back(subpass);
}

Vk_RenderPass::Vk_RenderPass(class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

Vk_RenderPass::~Vk_RenderPass()
{
}

CLOSE_NAMESPACE(Firestorm);