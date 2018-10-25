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

#include "VulkanPlatform.h"

#include <EASTL/optional.h>
#include <EASTL/vector.h>

#include "../IPipeline.h"
#include "../IRenderPass.h"
#include "../IShader.h"

OPEN_NAMESPACE(Firestorm);

using namespace eastl;

/**
	\brief A wrapper around the initialization and deinitialization of the graphics backend.

	The RenderSystem wraps the initialization and deinitialization of the graphics backend. In this version,
	we are wrapping the Vulkan backend.

	All things that are public should be made virtual and dumped into an interface.
 **/
class RenderSystem final
{
public:
	RenderSystem(class RenderMgr& renderMgr, class Window& window);
	~RenderSystem();

	void Initialize();
	void Shutdown();

	// creation functions.
	/**
		Creation function for a shader instance that is used by the resource loader routines.
	 **/
	bool MakeWhole(class IShader* shader, const IShader::CreateInfo& info);

	/**
		Creation function for a shader program instance that is used by the resource loader routines.
	**/
	bool MakeWhole(class IShaderProgram* shaderProgram, const IShaderProgram::CreateInfo& info);

	owner<class IShaderProgram*>  Make(const IShaderProgram::CreateInfo& info);
	owner<class IPipelineLayout*> Make(const IPipelineLayout::CreateInfo& info);
	owner<class IPipeline*>       Make(const IPipeline::CreateInfo& info);
	owner<class IRenderPass*>     Make(const IRenderPass::CreateInfo& info);

	// release functions.
	void Release(class Vk_Shader* shader);
	void Release(IPipeline* pipeline);
	void Release(IPipelineLayout* layout);
	void Release(IRenderPass* renderPass);

	// do not add to interface below this comment
	const VkExtent2D& GetSwapchainExtent() const { return _swapChainExtent; }

	size_t GetSwapchainImageFormat() const { return _swapChainImageFormat; }
private:
	void CheckValidationLayers();
	void CreateInstance();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();

	struct QueueFamilyIndices
	{
		eastl::optional<uint32_t> GraphicsFamily;
		eastl::optional<uint32_t> PresentFamily;
		bool IsComplete() const
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR   Capabilities;
		vector<VkSurfaceFormatKHR> Formats;
		vector<VkPresentModeKHR>   PresentModes;
	};

	bool IsDeviceSuitable(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	vector<const char*> GetRequiredExtensions() const;

	class RenderMgr& _renderMgr;
	class Window& _window;

	VkInstance _instance{ nullptr };
	VkDebugUtilsMessengerEXT _callback{ nullptr };
	VkPhysicalDevice _physicalDevice{ nullptr };
	VkDevice _device{ nullptr };
	VkSurfaceKHR _surface{ nullptr };
	VkQueue _graphicsQueue{ nullptr };
	VkQueue _presentQueue{ nullptr };

	vector<const char*> _deviceExtensions;
	VkSwapchainKHR _swapChain{ nullptr };
	vector<VkImage> _swapChainImages;
	VkFormat _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	vector<VkImageView> _swapChainImageViews;
#ifndef FIRE_FINAL
	vector<const char*> _validationLayers;
#endif
};

CLOSE_NAMESPACE(Firestorm);

#endif
