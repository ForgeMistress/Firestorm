///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IFramebuffer
//
//  Wrapper around a framebuffer. Interface derived from https://github.com/SaschaWillems/Vulkan
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_IFRAMEBUFFER_H_
#define LIBSCENE_IFRAMEBUFFER_H_
#pragma once

#include <libCore/Object.h>

#include "IRenderPass.h"
#include "IImage.h"

OPEN_NAMESPACE(Firestorm);

class IFramebuffer
{
	FIRE_MIRROR_DECLARE(IFramebuffer);
public:
	struct AttachmentCreateInfo
	{
		uint32_t           Width;
		uint32_t           Height;
		uint32_t           LayerCount;
		size_t             Format;
		ImageUsageFlagBits Usage;
	};

	virtual ~IFramebuffer() = default;

	virtual uint32_t AddAttachment(const AttachmentCreateInfo& info) = 0;
	virtual bool CreateSampler(Filter magFilter, Filter minFilter, SamplerAddressMode adressMode) = 0;
	virtual bool CreateRenderPass() = 0;
};

CLOSE_NAMESPACE(Firestorm);


#ifdef FIRE_GFX_VULKAN
#include "Vulkan/VkFramebuffer.h"
#endif

#endif