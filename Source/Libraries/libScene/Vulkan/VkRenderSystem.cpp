///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkRenderSystem
//
//  Implementation of the RenderSystem class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkRenderSystem.h"
#include <libCore/Logger.h>
#include "../RenderMgr.h"

#include <glfw/glfw3.h>

OPEN_NAMESPACE(Firestorm);

RenderSystem::RenderSystem(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Initialize()
{
	// if we're using Vulkan, we're using GLFW.
	uint32_t glfwExtCount = 0;
	const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	FIRE_LOG_DEBUG(":: %d Vulkan Extensions supported.", extensionCount);

	VkApplicationInfo appInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"",
		VK_MAKE_VERSION(1,0,0),
		"Firestorm",
		VK_MAKE_VERSION(0,0,1),
		VK_API_VERSION_1_1
	};
	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtCount;
	createInfo.ppEnabledExtensionNames = glfwExts;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
	FIRE_ASSERT_MSG(result == VK_SUCCESS, "failed to create vulkan instance");
}


CLOSE_NAMESPACE(Firestorm);