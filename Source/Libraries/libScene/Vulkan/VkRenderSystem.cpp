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

#include <libApp/Window.h>

#include <EASTL/sort.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                             const VkAllocationCallbacks*              pAllocator,
                                             VkDebugUtilsMessengerEXT*                 pCallback)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                          VkDebugUtilsMessengerEXT     callback,
                                          const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		func(instance, callback, pAllocator);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                   VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                   void*                                       pUserData)
{
	if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		FIRE_LOG_WARNING("!! [Vulkan WARNING]: %s", pCallbackData->pMessage);
	}
	else if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		FIRE_LOG_ERROR("!! [Vulkan ERROR]: %s", pCallbackData->pMessage);
	}
	else
	{
		FIRE_LOG_DEBUG("!! [Vulkan INFO]: %s", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

#define FIRE_VALIDATE_VK_CALL(function, ...)                                \
{                                                                           \
	VkResult result = function ( __VA_ARGS__ );                             \
	FIRE_ASSERT_MSG(result == VK_SUCCESS, "vulkan call "#function"failed"); \
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::RenderSystem(RenderMgr& renderMgr, Window& window)
: _renderMgr(renderMgr)
, _window(window)
, _deviceExtensions({
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
})
#ifndef FIRE_FINAL
, _validationLayers({
	"VK_LAYER_LUNARG_standard_validation"
	//, "VK_LAYER_LUNARG_api_dump"
})
#endif
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::~RenderSystem()
{
	Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Initialize()
{
	CheckValidationLayers();
	CreateInstance();
	SetupDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateGraphicsPipeline();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Shutdown()
{
	for(auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyDevice(_device, nullptr);
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
		VK_API_VERSION_1_0
	};
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// if we're using Vulkan, we're using GLFW.
	vector<const char*> extensions(GetRequiredExtensions());

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef FIRE_FINAL
	createInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
	createInfo.ppEnabledLayerNames = _validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
#endif

	FIRE_VALIDATE_VK_CALL(vkCreateInstance, &createInfo, nullptr, &_instance);
}

void RenderSystem::SetupDebugCallback()
{
#ifndef FIRE_FINAL
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	debugCreateInfo.pfnUserCallback = VulkanDebugCallback;
	debugCreateInfo.pUserData = nullptr; // Optional

	FIRE_VALIDATE_VK_CALL(CreateDebugUtilsMessengerEXT, _instance, &debugCreateInfo, nullptr, &_callback);
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

	using cpair = eastl::pair<int, VkPhysicalDevice>;
	vector<cpair> candidates;

	for(const auto& device : devices)
	{
		candidates.push_back(cpair(RateDeviceSuitability(device), device));
	}
	eastl::sort(candidates.begin(), candidates.end(),
		[](const cpair& left, const cpair& right) {
			return left.first > right.first;
		});
	_physicalDevice = (*candidates.begin()).second;
	FIRE_ASSERT_MSG(_physicalDevice != VK_NULL_HANDLE, "failed to find a suitable gpu");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
	vector<VkDeviceQueueCreateInfo> queueInfos;
	unordered_set<uint32_t> uniqueQueueFamilies = {
		indices.PresentFamily.value(),
		indices.GraphicsFamily.value()
	};

	float queuePriority = 1.0f;
	for(uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueInfo = {};
		queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueInfo.queueFamilyIndex = queueFamily;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = &queuePriority;
		queueInfos.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures features = {};
	
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.pQueueCreateInfos = queueInfos.data();
	deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	deviceInfo.pEnabledFeatures = &features;

	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(_deviceExtensions.size());
	deviceInfo.ppEnabledExtensionNames = _deviceExtensions.data();

#ifndef FIRE_FINAL
	deviceInfo.enabledLayerCount = static_cast<uint32_t>(_validationLayers.size());
	deviceInfo.ppEnabledLayerNames = _validationLayers.data();
#else
	deviceInfo.enabledLayerCount = 0;
#endif
	FIRE_VALIDATE_VK_CALL(vkCreateDevice, _physicalDevice, &deviceInfo, nullptr, &_device);

	vkGetDeviceQueue(_device, indices.GraphicsFamily.value(), 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, indices.PresentFamily.value(), 0, &_presentQueue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateSurface()
{
	FIRE_VALIDATE_VK_CALL(glfwCreateWindowSurface, _instance, _window.GetWindowHandle(), nullptr, &_surface);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
	if(swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.GraphicsFamily.value(), indices.PresentFamily.value()};

	if(indices.GraphicsFamily != indices.PresentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	FIRE_VALIDATE_VK_CALL(vkCreateSwapchainKHR, _device, &createInfo, nullptr, &_swapChain);

	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

	_swapChainImageFormat = surfaceFormat.format;
	_swapChainExtent = extent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateImageViews()
{
	_swapChainImageViews.resize(_swapChainImages.size());
	for(size_t i=0; i<_swapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapChainImageFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		FIRE_VALIDATE_VK_CALL(vkCreateImageView, _device, &createInfo, nullptr, &_swapChainImageViews[i]);

	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateGraphicsPipeline()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderSystem::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if(extensionsSupported)
	{
		SwapChainSupportDetails details = QuerySwapChainSupport(device);
		swapChainAdequate = !details.Formats.empty() && !details.PresentModes.empty();
	}
	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
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

	QueueFamilyIndices indices = FindQueueFamilies(device);
	if(!indices.IsComplete())
		score = 0;

	return score;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define has_flag(QFamily, BIT) (QFamily.queueFlags & BIT)
RenderSystem::QueueFamilyIndices RenderSystem::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i=0;
	for(const auto& queueFamily : queueFamilies)
	{
		if(queueFamily.queueCount > 0 && has_flag(queueFamily, VK_QUEUE_GRAPHICS_BIT))
		{
			indices.GraphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
		if(queueFamily.queueCount > 0 && presentSupport)
		{
			indices.PresentFamily = i;
		}

		if(indices.IsComplete())
		{
			break;
		}
		i++;
	}

	return indices;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderSystem::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
	unordered_set<string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

	for(const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::SwapChainSupportDetails RenderSystem::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
	if(formatCount != 0)
	{
		details.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.Formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
	if(presentModeCount != 0)
	{
		details.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.PresentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR RenderSystem::ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats)
{
	if(availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for(const auto& availableFormat : availableFormats)
	{
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkPresentModeKHR RenderSystem::ChooseSwapPresentMode(const vector<VkPresentModeKHR>& availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for(const auto& availablePresentMode : availablePresentModes) 
	{
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
		{
			return availablePresentMode;
		}
		else if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VkExtent2D RenderSystem::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		Vector2 size(_window.GetSize());
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(size.x),
			static_cast<uint32_t>(size.y)
		};

		actualExtent.width = eastl::max(capabilities.minImageExtent.width, eastl::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = eastl::max(capabilities.minImageExtent.height, eastl::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
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