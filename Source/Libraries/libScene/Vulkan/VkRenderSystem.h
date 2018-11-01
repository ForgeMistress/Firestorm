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
#include "../IImage.h"
#include "../ITexture.h"

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
	RenderSystem(class Application& app);
	~RenderSystem();

	void InitializeSystem();
	void Shutdown();

	// creation functions.
	/**
		Creation function for a shader instance that is used by the resource loader routines.
	 **/
	bool ResourceInitialize(class IShader* shader, const IShader::CreateInfo& info);

	/**
		Creation function for a shader program instance that is used by the resource loader routines.
	**/
	bool ResourceInitialize(class IShaderProgram* shaderProgram, const IShaderProgram::CreateInfo& info);

	eastl::shared_ptr<IShader>         CreateShader();
	eastl::shared_ptr<IShaderProgram>  CreateShaderProgram();
	eastl::shared_ptr<IPipelineLayout> CreatePipelineLayout();
	eastl::shared_ptr<IPipeline>       CreatePipeline();
	eastl::shared_ptr<IRenderPass>     CreateRenderPass();
	eastl::shared_ptr<ITexture>        CreateTexture();

	owner<class IPipelineLayout*> Make(const IPipelineLayout::CreateInfo& info);
	owner<class IPipeline*>       Make(const IPipeline::CreateInfo& info);
	owner<class IRenderPass*>     Make(const IRenderPass::CreateInfo& info);

	// release functions.
	void Release(IShader* shader);
	void Release(IPipeline* pipeline);
	void Release(IPipelineLayout* layout);
	void Release(IRenderPass* renderPass);

	// do not add to interface below this comment
	const VkExtent2D& GetSwapchainExtent() const { return _swapChainExtent; }

	size_t GetSwapchainImageFormat() const { return _swapChainImageFormat; }



	VkInstance GetInstance() const { return _instance; }
	VkPhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
	VkDevice GetDevice() const { return _device; }
	VkQueue GetGraphicsQueue() const { return _graphicsQueue; }
	VkQueue GetPresentQueue() const { return _presentQueue; }

	uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound = nullptr);

private:
	void RegisterResourceMakers();

	void CheckValidationLayers();
	void CreateInstance();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateFramebuffers();

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

	class Application& _app;
	class RenderMgr& _renderMgr;
	class Window& _window;
	class ResourceMgr& _resourceMgr;

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
	vector<VkFramebuffer> _swapChainFrameBuffers;
#ifndef FIRE_FINAL
	vector<const char*> _validationLayers;
#endif

	VkPhysicalDeviceMemoryProperties _memoryProperties;
};

CLOSE_NAMESPACE(Firestorm);

#endif
