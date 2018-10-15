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

	bool IsDeviceSuitable(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);

	struct QueueFamilyIndices
	{
		eastl::optional<uint32_t> _graphicsFamily;
		bool IsComplete()
		{
			return _graphicsFamily.has_value();
		}
	};

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
#ifndef FIRE_FINAL
	vector<const char*> _validationLayers;
#endif
};

CLOSE_NAMESPACE(Firestorm);

#endif
