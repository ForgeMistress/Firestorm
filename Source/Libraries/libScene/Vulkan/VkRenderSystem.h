///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  VkRenderSystem
//
//  Implementation of the RenderSystem class for the Vulkan backend.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKRENDERSYSTEM_H_
#define LIBSCENE_VKRENDERSYSTEM_H_
#pragma once

#include <vulkan/vulkan.h>
#include <EASTL/optional.h>
OPEN_NAMESPACE(Firestorm);

/**
	\brief A wrapper around the initialization and deinitialization of the graphics backend.

	The RenderSystem wraps the initialization and deinitialization of the graphics backend. In this version,
	we are wrapping the Vulkan backend.
 **/
class RenderSystem final
{
public:
	RenderSystem(class RenderMgr& renderMgr);
	~RenderSystem();

	void Initialize();
	void Shutdown();
private:
	void CheckValidationLayers();
	void CreateInstance();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();

	struct QueueFamilyIndices
	{
		eastl::optional<uint32_t> GraphicsFamily;
		bool IsComplete() const
		{
			return GraphicsFamily.has_value();
		}
	};

	bool IsDeviceSuitable(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	vector<const char*> GetRequiredExtensions() const;

	class RenderMgr& _renderMgr;
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _callback;
	VkPhysicalDevice _physicalDevice;
	VkDevice _device;
#ifndef FIRE_FINAL
	vector<const char*> _validationLayers;
#endif
};

CLOSE_NAMESPACE(Firestorm);

#endif
