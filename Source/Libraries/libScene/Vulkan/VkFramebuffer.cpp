///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkFramebuffer
//
//  Implementation of the IFramebuffer class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkFramebuffer.h"
#include "VkRenderPass.h"
#include "VulkanPlatform.h"
#include "../IFramebuffer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

bool Vk_FramebufferAttachment::HasDepth()
{
	vector<VkFormat> formats = 
	{
		VK_FORMAT_D16_UNORM,
		VK_FORMAT_X8_D24_UNORM_PACK32,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
	};
	return find(formats.begin(), formats.end(), Format) != std::end(formats);
}

bool Vk_FramebufferAttachment::HasStencil()
{
	vector<VkFormat> formats = 
	{
		VK_FORMAT_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
	};
	return find(formats.begin(), formats.end(), Format) != std::end(formats);
}

bool Vk_FramebufferAttachment::IsDepthStencil()
{
	return(HasDepth() || HasStencil());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IFramebuffer::CreateInfo IFramebuffer::MakeCreateInfo(RenderSystem& renderSystem)
{
	IFramebuffer::CreateInfo createInfo;
	return createInfo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_Framebuffer::Vk_Framebuffer(RenderSystem& system)
: _renderSystem(system)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_Framebuffer::~Vk_Framebuffer()
{
	for(auto attachment : _attachments)
	{
		vkDestroyImage(_renderSystem.GetDevice(), attachment.Image, nullptr);
		vkDestroyImageView(_renderSystem.GetDevice(), attachment.View, nullptr);
		vkFreeMemory(_renderSystem.GetDevice(), attachment.Memory, nullptr);
	}
	vkDestroySampler(_renderSystem.GetDevice(), _vkSampler, nullptr);
	//vkDestroyRenderPass(_renderSystem.GetDevice(), _vkRenderPass, nullptr);
	vkDestroyFramebuffer(_renderSystem.GetDevice(), _vkFramebuffer, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t Vk_Framebuffer::AddAttachment(const AttachmentCreateInfo& info)
{
	Vk_FramebufferAttachment attachment;

	attachment.Format = static_cast<VkFormat>(info.Format);

	VkImageAspectFlags aspectMask = 0;//VK_FLAGS_NONE;

	// Select aspect mask and layout depending on usage

	// Color attachment
	if(info.Usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	// Depth (and/or stencil) attachment
	if(info.Usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		if(attachment.HasDepth())
		{
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		if(attachment.HasStencil())
		{
			aspectMask = aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}

	assert(aspectMask > 0);

	VkImageCreateInfo image = initializers::imageCreateInfo();
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = static_cast<VkFormat>(info.Format);
	image.extent.width = info.Width;
	image.extent.height = info.Height;
	image.extent.depth = 1;
	image.mipLevels = 1;
	image.arrayLayers = info.LayerCount;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = info.Usage;

	VkMemoryAllocateInfo memAlloc = initializers::memoryAllocateInfo();
	VkMemoryRequirements memReqs;

	// Create image for this attachment
	FIRE_VK_VALIDATE(vkCreateImage(_renderSystem.GetDevice(), &image, nullptr, &attachment.Image));
	vkGetImageMemoryRequirements(_renderSystem.GetDevice(), attachment.Image, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	memAlloc.memoryTypeIndex = _renderSystem.GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	FIRE_VK_VALIDATE(vkAllocateMemory(_renderSystem.GetDevice(), &memAlloc, nullptr, &attachment.Memory));
	FIRE_VK_VALIDATE(vkBindImageMemory(_renderSystem.GetDevice(), attachment.Image, attachment.Memory, 0));

	attachment.SubresourceRange = {};
	attachment.SubresourceRange.aspectMask = aspectMask;
	attachment.SubresourceRange.levelCount = 1;
	attachment.SubresourceRange.layerCount = info.LayerCount;

	VkImageViewCreateInfo imageView = initializers::imageViewCreateInfo();
	imageView.viewType = (info.LayerCount == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	imageView.format = static_cast<VkFormat>(info.Format);
	imageView.subresourceRange = attachment.SubresourceRange;
	//todo: workaround for depth+stencil attachments
	imageView.subresourceRange.aspectMask = (attachment.HasDepth()) ? VK_IMAGE_ASPECT_DEPTH_BIT : aspectMask;
	imageView.image = attachment.Image;
	FIRE_VK_VALIDATE(vkCreateImageView(_renderSystem.GetDevice(), &imageView, nullptr, &attachment.View));

	// Fill attachment description
	attachment.Description = {};
	attachment.Description.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment.Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.Description.storeOp = (info.Usage & VK_IMAGE_USAGE_SAMPLED_BIT) ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.Description.format = static_cast<VkFormat>(info.Format);
	attachment.Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// Final layout
	// If not, final layout depends on attachment type
	if(attachment.HasDepth() || attachment.HasStencil())
	{
		attachment.Description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	}
	else
	{
		attachment.Description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	_attachments.push_back(attachment);

	return static_cast<uint32_t>(_attachments.size() - 1);
}

bool Vk_Framebuffer::CreateSampler(Filter magFilter, Filter minFilter, SamplerAddressMode adressMode)
{
	VkSamplerCreateInfo samplerInfo = initializers::samplerCreateInfo();
	samplerInfo.magFilter = static_cast<VkFilter>(magFilter);
	samplerInfo.minFilter = static_cast<VkFilter>(minFilter);
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(adressMode);
	samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(adressMode);
	samplerInfo.addressModeW = static_cast<VkSamplerAddressMode>(adressMode);
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	return vkCreateSampler(_renderSystem.GetDevice(), &samplerInfo, nullptr, &_vkSampler) == VK_SUCCESS;
}

bool Vk_Framebuffer::CreateRenderPass()
{
	vector<VkAttachmentDescription> attachmentDescriptions;
	for(auto& attachment : _attachments)
	{
		attachmentDescriptions.push_back(attachment.Description);
	}

	// Collect attachment references
	std::vector<VkAttachmentReference> colorReferences;
	VkAttachmentReference depthReference = {};
	bool hasDepth = false; 
	bool hasColor = false;

	uint32_t attachmentIndex = 0;

	for(auto& attachment : _attachments)
	{
		if(attachment.IsDepthStencil())
		{
			// Only one depth attachment allowed
			assert(!hasDepth);
			depthReference.attachment = attachmentIndex;
			depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			hasDepth = true;
		}
		else
		{
			colorReferences.push_back({ attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
			hasColor = true;
		}
		attachmentIndex++;
	};

	// Default render pass setup uses only one subpass
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	if(hasColor)
	{
		subpass.pColorAttachments = colorReferences.data();
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	}
	if(hasDepth)
	{
		subpass.pDepthStencilAttachment = &depthReference;
	}

	// Use subpass dependencies for attachment layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create render pass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.pAttachments = attachmentDescriptions.data();
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 2;
	renderPassInfo.pDependencies = dependencies.data();
	FIRE_VK_VALIDATE(vkCreateRenderPass(_renderSystem.GetDevice(), &renderPassInfo, nullptr, &_vkRenderPass));

	std::vector<VkImageView> attachmentViews;
	for(auto attachment : _attachments)
	{
		attachmentViews.push_back(attachment.View);
	}

	// Find. max number of layers across attachments
	uint32_t maxLayers = 0;
	for(auto attachment : _attachments)
	{
		if(attachment.SubresourceRange.layerCount > maxLayers)
		{
			maxLayers = attachment.SubresourceRange.layerCount;
		}
	}

	VkFramebufferCreateInfo framebufferInfo = {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = _vkRenderPass;
	framebufferInfo.pAttachments = attachmentViews.data();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
	framebufferInfo.width = _width;
	framebufferInfo.height = _height;
	framebufferInfo.layers = maxLayers;
	FIRE_VK_VALIDATE(vkCreateFramebuffer(_renderSystem.GetDevice(), &framebufferInfo, nullptr, &_vkFramebuffer));

	return true;
}

CLOSE_NAMESPACE(Firestorm);