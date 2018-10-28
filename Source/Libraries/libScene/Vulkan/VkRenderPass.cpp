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

void IRenderPass::CreateInfo::AddAttachmentDesc(const IRenderPass::CreateInfo::AttachmentDescription& attachmentDesc)
{
	Attachments.push_back(attachmentDesc);
}

void IRenderPass::CreateInfo::AddSubpass(const SubpassDescription& subpass)
{
	Subpasses.push_back(subpass);
}

IRenderPass::CreateInfo IRenderPass::MakeCreateInfo(const class RenderSystem& renderSystem)
{
	IRenderPass::CreateInfo renderPassInfo;

	IRenderPass::CreateInfo::AttachmentDescription attachDesc{};
	attachDesc.Flags = static_cast<AttachmentDescriptionFlags>(0);
	attachDesc.Format = renderSystem.GetSwapchainImageFormat();
	attachDesc.FinalLayout = ImageLayout::kPRESENT_SRC;
	renderPassInfo.AddAttachmentDesc(attachDesc);

	IRenderPass::CreateInfo::AttachmentReference colorRef;
	colorRef.Attachment = 0;
	colorRef.Layout = ImageLayout::kCOLOR_ATTACHMENT_OPTIMAL;

	IRenderPass::CreateInfo::SubpassDescription subpass;
	subpass.ColorAttachments.push_back(colorRef);

	return renderPassInfo;
}

Vk_RenderPass::Vk_RenderPass(class RenderSystem& renderSystem)
: _renderSystem(renderSystem)
{
}

Vk_RenderPass::~Vk_RenderPass()
{
}

CLOSE_NAMESPACE(Firestorm);