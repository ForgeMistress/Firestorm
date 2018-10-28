///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkFramebuffer
//
//  Implementation of the IFramebuffer class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKFRAMEBUFFER_H_
#define LIBSCENE_VKFRAMEBUFFER_H_
#pragma once

#include "../RenderMgr.h"
#include "../IFramebuffer.h"

OPEN_NAMESPACE(Firestorm);

// borrowed from https://github.com/SaschaWillems/Vulkan
struct Vk_FramebufferAttachment
{
	VkImage Image;
	VkDeviceMemory Memory;
	VkImageView View;
	VkFormat Format;
	VkImageSubresourceRange SubresourceRange;
	VkAttachmentDescription Description;

	bool HasDepth();
	bool HasStencil();
	bool IsDepthStencil();
};

class Vk_Framebuffer : public IFramebuffer
{
	FIRE_MIRROR_DECLARE(Vk_Framebuffer, IFramebuffer);
public:
	Vk_Framebuffer(RenderSystem& system);
	virtual ~Vk_Framebuffer();

	uint32_t AddAttachment(const AttachmentCreateInfo& info) override;
	bool CreateSampler(Filter magFilter, Filter minFilter, SamplerAddressMode adressMode) override;
	bool CreateRenderPass() override;
private:
	class RenderSystem&              _renderSystem;
	uint32_t                         _width;
	uint32_t                         _height;
	VkFramebuffer                    _vkFramebuffer;
	VkRenderPass                     _vkRenderPass;
	VkSampler                        _vkSampler;
	vector<Vk_FramebufferAttachment> _attachments;
};

CLOSE_NAMESPACE(Firestorm);

#endif