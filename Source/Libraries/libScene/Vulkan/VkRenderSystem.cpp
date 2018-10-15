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

#include <EASTL/sort.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		func(instance, callback, pAllocator);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::RenderSystem(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
#ifndef FIRE_FINAL
, _validationLayers({
	"VK_LAYER_LUNARG_standard_validation"
})
#endif
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::~RenderSystem()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Initialize()
{
	CheckValidationLayers();
	CreateInstance();
	SetupDebugCallback();
	PickPhysicalDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Shutdown()
{
#ifndef FIRE_FINAL
	DestroyDebugUtilsMessengerEXT(_instance, _callback, nullptr);
#endif
	vkDestroyInstance(_instance, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CheckValidationLayers()
{	
#ifndef FIRE_FINAL
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for(const auto& layerName : _validationLayers)
	{
		bool found = false;
		for(const auto& layerProperties : availableLayers)
		{
			FIRE_LOG_DEBUG("Layer: %s", layerProperties.layerName);
			if(string(layerName) == layerProperties.layerName)
			{
				found = true;
				break;
			}
		}
		FIRE_ASSERT_MSG(found, Format("failed to find validation layer %s", layerName).c_str());
	}
#endif
}

void RenderSystem::CreateInstance()
{
	VkApplicationInfo appInfo{
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"",
		VK_MAKE_VERSION(1,0,0),
		"Firestorm",
		VK_MAKE_VERSION(0,0,1),
		VK_API_VERSION_1_1
	};
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// if we're using Vulkan, we're using GLFW.
	vector<const char*> extensions(GetRequiredExtensions());

	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef FIRE_FINAL
	createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
	createInfo.ppEnabledLayerNames = _validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);
	FIRE_ASSERT_MSG(result == VK_SUCCESS, "failed to create vulkan instance");
}

void RenderSystem::SetupDebugCallback()
{
#ifndef FIRE_FINAL
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	debugCreateInfo.pfnUserCallback = RenderSystem::DebugCallback;
	debugCreateInfo.pUserData = nullptr; // Optional

	if(CreateDebugUtilsMessengerEXT(_instance, &debugCreateInfo, nullptr, &_callback) != VK_SUCCESS)
	{
		FIRE_ASSERT(false);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
	FIRE_ASSERT_MSG(deviceCount > 0, "there were no physical devices found");

	vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
	eastl::vector<eastl::pair<int, VkPhysicalDevice>> candidates;

	for(const auto& device : devices)
	{
		candidates.push_back(eastl::pair(RateDeviceSuitability(device), device));
	}
	candidates.push_back(eastl::pair(0, nullptr));
	candidates.push_back(eastl::pair(1, nullptr));
	candidates.push_back(eastl::pair(2, nullptr));
	eastl::sort(candidates.begin(), candidates.end(), 
		[](const eastl::pair<int, VkPhysicalDevice>& left, const eastl::pair<int, VkPhysicalDevice>& right) {
			FIRE_LOG_DEBUG("scores %d/%d", left.first, right.first);
			return left.first > right.first;
		});
	FIRE_LOG_DEBUG("score of best pick: %d", (*candidates.begin()).first);
	_physicalDevice = (*candidates.begin()).second;
	FIRE_ASSERT_MSG(_physicalDevice != VK_NULL_HANDLE, "failed to find a suitable gpu");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderSystem::IsDeviceSuitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &props);
	vkGetPhysicalDeviceFeatures(device, &features);
	return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && features.geometryShader;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int RenderSystem::RateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &props);
	vkGetPhysicalDeviceFeatures(device, &features);

	int score = 0;
	if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}
	score += props.limits.maxImageDimension2D;
	if(!features.geometryShader)
	{
		return 0;
	}
	return score;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL RenderSystem::DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		FIRE_LOG_WARNING("!! Vulkan Warning: %s", pCallbackData->pMessage);
	}
	if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		FIRE_LOG_ERROR("!! Vulkan ERROR: %s", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<const char*> RenderSystem::GetRequiredExtensions() const
{
	uint32_t glfwExtCount = 0;
	const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
	vector<const char*> extensions(glfwExts, glfwExts + glfwExtCount);
#ifndef FIRE_FINAL
	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
	return extensions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);