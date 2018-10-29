///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkRenderPass
//
//  Implementation of the RenderPass class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKRENDERPASS_H_
#define LIBSCENE_VKRENDERPASS_H_
#pragma once

#include "../RenderMgr.h"

#include "../IRenderPass.h"

#include "VkRenderSystem.h"

#include <libMath/Vector.h>

OPEN_NAMESPACE(Firestorm);

class Vk_RenderPass final : public IRenderPass
{
public:
	Vk_RenderPass(class RenderSystem& renderSystem);
	virtual ~Vk_RenderPass();

private:
	friend class RenderSystem;
	class RenderSystem& _renderSystem;
	VkRenderPass _vkRenderPass;
};

CLOSE_NAMESPACE(Firestorm);

#endif