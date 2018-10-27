///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IFramebuffer
//
//  IFramebuffer
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_IFRAMEBUFFER_H_
#define LIBSCENE_IFRAMEBUFFER_H_
#pragma once

#include <libCore/Object.h>

#include "IRenderPass.h"

OPEN_NAMESPACE(Firestorm);

class IFramebuffer
{
	FIRE_MIRROR_DECLARE(IFramebuffer);
public:
	struct CreateInfo
	{
		const void*                 pNext;
		VkFramebufferCreateFlags    flags;
		IRenderPass*                RenderPass;
		uint32_t                    attachmentCount;
		const VkImageView*          pAttachments;
		uint32_t                    width;
		uint32_t                    height;
		uint32_t                    layers;
	};
	virtual ~IFramebuffer() = default;

private:
};

CLOSE_NAMESPACE(Firestorm);


#ifdef FIRE_GFX_VULKAN
#include "Vulkan/VkFramebuffer.h"
#endif

#endif