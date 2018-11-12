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

#include "../RenderMgr.h"
#include "../Enumerations.h"

#include "VkRenderSystem.h"
#include "VkShaderProgram.h"
#include "VkPipeline.h"
#include "VkRenderPass.h"
#include "Vk_Texture.h"

#include <libCore/Logger.h>

#include <libApp/Application.h>
#include <libApp/Window.h>

#include <EASTL/sort.h>

#define FIRE_VK_ALLOC nullptr
#define FIRE_VK_FREE nullptr

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



namespace {

VkPolygonMode map(PolyMode mode)
{
	static VkPolygonMode polyModes[] ={
		VK_POLYGON_MODE_FILL,// kFill,
		VK_POLYGON_MODE_LINE,// kLine,
		VK_POLYGON_MODE_POINT,// kPoint
	};
	return polyModes[(uint8_t)mode];
}

VkViewport map(const IPipeline::CreateInfo::Viewport& viewport)
{
	return VkViewport{
		viewport.PosY,
		viewport.PosX,
		viewport.ExtentX,
		viewport.ExtentY,
		viewport.MinDepth,
		viewport.MaxDepth
	};
}

VkRect2D map(const IPipeline::CreateInfo::Scissor& scissor)
{
	VkRect2D rect;
	rect.offset = VkOffset2D{scissor.OffsetX, scissor.OffsetY};
	rect.extent = VkExtent2D{scissor.ExtentX, scissor.ExtentY};
	return rect;
}

VkPipelineShaderStageCreateInfo map(const Resource<IShader>& shader)
{
	VkPipelineShaderStageCreateInfo stageInfo = {};
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = static_cast<VkShaderStageFlagBits>(shader->GetType());
	stageInfo.module = shader.get<Vk_Shader>()->GetModule();
	stageInfo.pName = "main";
	return stageInfo;
}

void print(const VkAttachmentDescription& desc)
{
	FIRE_LOG_DEBUG("=== VkAttachmentDescription ===");
	FIRE_LOG_DEBUG("    format:         %d", desc.format);
	FIRE_LOG_DEBUG("    samples:        %d", desc.samples);
	FIRE_LOG_DEBUG("    loadOp:         %d", desc.loadOp);
	FIRE_LOG_DEBUG("    storeOp:        %d", desc.storeOp);
	FIRE_LOG_DEBUG("    stencilLoadOp:  %d", desc.stencilLoadOp);
	FIRE_LOG_DEBUG("    stencilStoreOp: %d", desc.stencilStoreOp);
	FIRE_LOG_DEBUG("    initialLayout:  %d", desc.initialLayout);
	FIRE_LOG_DEBUG("    finalLayout:    %d", desc.finalLayout);
}

vector<VkAttachmentReference> map(const vector<IRenderPass::CreateInfo::AttachmentReference>& refs)
{
	vector<VkAttachmentReference> outrefs;
	outrefs.reserve(refs.size());
	for(size_t i=0; i<refs.size(); ++i)
	{
		outrefs.push_back(VkAttachmentReference{
			refs[i].Attachment,
			static_cast<VkImageLayout>(refs[i].Layout)
		});
	}
	return outrefs;
}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderSystem::RenderSystem(Application& app)
: _app(app)
, _renderMgr(app.GetSystems().GetRenderMgr())
, _window(app.GetSystems().GetWindow())
, _resourceMgr(app.GetSystems().GetResourceMgr())
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

void RenderSystem::InitializeSystem()
{
	RegisterResourceMakers();

	CheckValidationLayers();
	CreateInstance();
	SetupDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Shutdown()
{
	for(auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView(_device, imageView, FIRE_VK_FREE);
	}
	vkDestroySwapchainKHR(_device, _swapChain, FIRE_VK_FREE);
	vkDestroySurfaceKHR(_instance, _surface, FIRE_VK_FREE);
	vkDestroyDevice(_device, FIRE_VK_FREE);
#ifndef FIRE_FINAL
	DestroyDebugUtilsMessengerEXT(_instance, _callback, FIRE_VK_FREE);
#endif
	vkDestroyInstance(_instance, FIRE_VK_FREE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderSystem::ResourceInitialize(IShader* sshader, const IShader::CreateInfo& info)
{
	FIRE_ASSERT_MSG(sshader, "need to pass in an allocated shader b-baka");
	Vk_Shader* shader = static_cast<Vk_Shader*>(sshader);

	VkShaderModuleCreateInfo shaderModule ={};
	shaderModule.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModule.codeSize = info.Data.size();
	shaderModule.pCode = reinterpret_cast<const uint32_t*>(info.Data.data());
	shader->_type = info.Type;

	FIRE_VK_VALIDATE(vkCreateShaderModule(_device, &shaderModule, FIRE_VK_ALLOC, &shader->_vkShader));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderSystem::ResourceInitialize(class IShaderProgram* shaderProgram, const IShaderProgram::CreateInfo& info)
{
	Vk_ShaderProgram* prog = static_cast<Vk_ShaderProgram*>(shaderProgram);
	prog->_shaders = info.Shaders;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IShader> RenderSystem::CreateShader()
{
	return shared_ptr<IShader>(new Vk_Shader(*this), [this](IShader* p) {
		Vk_Shader* ptr = static_cast<Vk_Shader*>(p);
		if(ptr->_vkShader)
			vkDestroyShaderModule(_device, ptr->_vkShader, FIRE_VK_FREE);
		delete ptr;
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IShaderProgram> RenderSystem::CreateShaderProgram()
{
	return shared_ptr<IShaderProgram>(new Vk_ShaderProgram(*this), [this](IShaderProgram* p) {
		delete p;
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IShaderProgram> RenderSystem::CreateShaderProgram(const IShaderProgram::CreateInfo& info)
{
	return nullptr;
	//return shared_ptr<IShaderProgram>(Make(info), [this](IShaderProgram* p) {
	//	delete p;
	//});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IPipelineLayout> RenderSystem::CreatePipelineLayout()
{
	return shared_ptr<IPipelineLayout>(new Vk_PipelineLayout(*this), [this](IPipelineLayout* p) {
		Vk_PipelineLayout* ptr = static_cast<Vk_PipelineLayout*>(p);
		if(ptr->_vkLayout)
			vkDestroyPipelineLayout(_device, ptr->_vkLayout, FIRE_VK_FREE);
		delete p;
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IPipelineLayout> RenderSystem::CreatePipelineLayout(const IPipelineLayout::CreateInfo& info)
{
	return shared_ptr<IPipelineLayout>(Make(info), [this](IPipelineLayout* p) {
		Vk_PipelineLayout* ptr = static_cast<Vk_PipelineLayout*>(p);
		if(ptr->_vkLayout)
			vkDestroyPipelineLayout(_device, ptr->_vkLayout, FIRE_VK_FREE);
		delete p;
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IPipeline> RenderSystem::CreatePipeline()
{
	return shared_ptr<IPipeline>(new Vk_Pipeline(*this), [this](IPipeline* p) {
		Vk_Pipeline* ptr = static_cast<Vk_Pipeline*>(p);
		if(ptr->_vkPipeline)
			vkDestroyPipeline(_device, ptr->_vkPipeline, FIRE_VK_FREE);
		delete p;
	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<IRenderPass> RenderSystem::CreateRenderPass()
{
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<ITexture> RenderSystem::CreateTexture()
{
	return shared_ptr<ITexture>(new Vk_Texture(*this), [this](ITexture* p) {

	});
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IPipelineLayout* RenderSystem::Make(const IPipelineLayout::CreateInfo& info)
{
	Vk_PipelineLayout* layout = new Vk_PipelineLayout(*this);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;//info.Layout.size();
	pipelineLayoutInfo.pushConstantRangeCount = 0; //info.constantrangecount;
	vkCreatePipelineLayout(_device, &pipelineLayoutInfo, FIRE_VK_ALLOC, &layout->_vkLayout);

	return layout;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IPipeline* RenderSystem::Make(const IPipeline::CreateInfo& info)
{
	FIRE_ASSERT_MSG(info.ShaderStage.IsValid(), "no shader stage was specified for this pipeline");
	FIRE_ASSERT_MSG(info.Layout != nullptr,     "no pipeline layout was specified for this pipeline");
	FIRE_ASSERT_MSG(info.RenderPass != nullptr, "no render pass was specified for this pipeline");
	// whoof... ok, here we go... let's do this...

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineShaderStageCreateInfo
	const Vk_ShaderProgram* shaderProgram = info.ShaderStage.get<Vk_ShaderProgram>();
	size_t numShaders = shaderProgram->_shaders.size();
	vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.reserve(numShaders);
	for(size_t i=0; i<shaderProgram->_shaders.size(); ++i)
	{
		shaderStages.push_back(map(shaderProgram->_shaders[i]));
	}
	// END VkPipelineShaderStageCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineVertexInputStateCreateInfo
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	// END VkPipelineVertexInputStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineInputAssemblyStateCreateInfo
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	// inputAssembly.flags = info.InputAssembly.Flags;
	inputAssembly.topology = static_cast<VkPrimitiveTopology>(info.InputAssembly.Topology);
	inputAssembly.primitiveRestartEnable = info.InputAssembly.PrimitiveRestartEnable;
	// END VkPipelineViewportStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineViewportStateCreateInfo
	vector<VkViewport> viewports;
	viewports.reserve(info.Viewports.size());
	for(size_t i=0; i<info.Viewports.size(); ++i)
	{
		viewports.push_back(map(info.Viewports[i]));
	}
	vector<VkRect2D> scissors;
	scissors.reserve(info.Scissors.size());
	for(size_t i=0; i<info.Scissors.size(); ++i)
	{
		scissors.push_back(map(info.Scissors[i]));
	}
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = narrow_cast<uint32_t>(viewports.size());
	viewportState.pViewports = viewports.data();
	viewportState.scissorCount = narrow_cast<uint32_t>(scissors.size());
	viewportState.pScissors = scissors.data();
	// END VkPipelineMultisampleStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineRasterizationStateCreateInfo
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = info.RasterizerState.DepthClampEnable;
	rasterizer.rasterizerDiscardEnable = info.RasterizerState.RasterizerDiscardEnable;
	rasterizer.polygonMode = map(info.RasterizerState.PolygonMode);
	rasterizer.lineWidth = info.RasterizerState.LineWidth;
	rasterizer.cullMode = static_cast<VkCullModeFlags>(info.RasterizerState.CullMode);
	rasterizer.frontFace = static_cast<VkFrontFace>(info.RasterizerState.FrontFace);
	rasterizer.depthBiasEnable = info.RasterizerState.DepthBiasEnable;
	// END VkPipelineMultisampleStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineMultisampleStateCreateInfo
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = info.MultisampleState.SampleShadingEnable;
	multisampling.rasterizationSamples = static_cast<VkSampleCountFlagBits>(info.MultisampleState.RasterSamples);
	// END VkPipelineMultisampleStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VkPipelineColorBlendStateCreateInfo
	vector<VkPipelineColorBlendAttachmentState> attachmentStates(info.PipelineColorBlendState.Attachments.size());
	for(size_t i=0; i<info.PipelineColorBlendState.Attachments.size(); ++i)
	{
		const auto& state = info.PipelineColorBlendState.Attachments[i];
		attachmentStates[i].alphaBlendOp = static_cast<VkBlendOp>(state.AlphaBlendOp);
		attachmentStates[i].blendEnable = state.BlendEnable;
		attachmentStates[i].colorBlendOp = static_cast<VkBlendOp>(state.ColorBlendOp);
		attachmentStates[i].colorWriteMask = state.ColorWriteMask;
		attachmentStates[i].dstAlphaBlendFactor = static_cast<VkBlendFactor>(state.DstAlphaBlendFactor);
		attachmentStates[i].dstColorBlendFactor = static_cast<VkBlendFactor>(state.DstColorBlendFactor);
		attachmentStates[i].srcAlphaBlendFactor = static_cast<VkBlendFactor>(state.SrcAlphaBlendFactor);
		attachmentStates[i].srcColorBlendFactor = static_cast<VkBlendFactor>(state.SrcColorBlendFactor);
	}

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = info.PipelineColorBlendState.LogicOpEnable;
	colorBlending.logicOp = static_cast<VkLogicOp>(info.PipelineColorBlendState.LogicOp);
	colorBlending.attachmentCount = narrow_cast<uint32_t>(attachmentStates.size());
	colorBlending.pAttachments = attachmentStates.data();
	eastl::copy(
		&info.PipelineColorBlendState.BlendConstants[0],
		&info.PipelineColorBlendState.BlendConstants[4], 
		colorBlending.blendConstants);
	// END VkPipelineColorBlendStateCreateInfo
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = narrow_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = static_cast<Vk_PipelineLayout*>(info.Layout)->_vkLayout;
	pipelineInfo.renderPass = static_cast<Vk_RenderPass*>(info.RenderPass)->_vkRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	Vk_Pipeline* pipeline = new Vk_Pipeline(*this);
	FIRE_VK_VALIDATE(
		vkCreateGraphicsPipelines(
			_device, 
			VK_NULL_HANDLE, 
			1, 
			&pipelineInfo, 
			FIRE_VK_ALLOC, 
			&pipeline->_vkPipeline
		)
	);
	
	return pipeline;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IRenderPass* RenderSystem::Make(const IRenderPass::CreateInfo& info)
{
	vector<VkAttachmentDescription> attachments;
	attachments.reserve(info.Attachments.size());
	for(size_t i=0; i<info.Attachments.size(); ++i)
	{
		const auto& a = info.Attachments[i];
		attachments.push_back(VkAttachmentDescription{
			static_cast<VkAttachmentDescriptionFlags>(a.Flags),
			static_cast<VkFormat>(a.Format),
			static_cast<VkSampleCountFlagBits>(a.Samples),
			static_cast<VkAttachmentLoadOp>(a.LoadOp),
			static_cast<VkAttachmentStoreOp>(a.StoreOp),
			static_cast<VkAttachmentLoadOp>(a.StencilLoadOp),
			static_cast<VkAttachmentStoreOp>(a.StencilStoreOp),
			static_cast<VkImageLayout>(a.InitialLayout),
			static_cast<VkImageLayout>(a.FinalLayout)
		});
	}

	vector<vector<VkAttachmentReference>> attachmentRefs;
	size_t numSubpasses = info.Subpasses.size();
	vector<VkSubpassDescription> subpasses;
	subpasses.reserve(numSubpasses);
	for(size_t i=0; i<numSubpasses; ++i)
	{
		const auto& subpassInfo = info.Subpasses[i];
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = static_cast<VkPipelineBindPoint>(info.Subpasses[i].PipelineBindPoint);

		if(!subpassInfo.ColorAttachments.empty())
		{
			attachmentRefs.push_back(map(subpassInfo.ColorAttachments));
			subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.back().size());
			subpass.pColorAttachments = attachmentRefs.back().data();
		}

		if(!subpassInfo.InputAttachments.empty())
		{
			attachmentRefs.push_back(map(subpassInfo.InputAttachments));
			subpass.inputAttachmentCount = static_cast<uint32_t>(attachmentRefs.back().size());
			subpass.pInputAttachments = attachmentRefs.back().data();
		}

		if(!subpassInfo.ResolveAttachments.empty())
		{
			attachmentRefs.push_back(map(subpassInfo.ResolveAttachments));
			subpass.pResolveAttachments = attachmentRefs.back().data();
		}

		if(!subpassInfo.DepthStencilAttachments.empty())
		{
			attachmentRefs.push_back(map(subpassInfo.DepthStencilAttachments));
			subpass.pDepthStencilAttachment = attachmentRefs.back().data();
			subpass.preserveAttachmentCount = static_cast<uint32_t>(attachmentRefs.back().size());
		}

		if(!subpassInfo.PreserveAttachments.empty())
		{
			subpass.pPreserveAttachments = subpassInfo.PreserveAttachments.data();
		}

		subpasses.push_back(subpass);
	}

	VkRenderPassCreateInfo renderPassInfo ={};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
	renderPassInfo.pSubpasses = subpasses.data();

	Vk_RenderPass* renderPass = new Vk_RenderPass(*this);
	FIRE_VK_VALIDATE(vkCreateRenderPass(_device, &renderPassInfo, FIRE_VK_ALLOC, &renderPass->_vkRenderPass));
	return renderPass;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Release(IShader* shader)
{
	vkDestroyShaderModule(_device, static_cast<Vk_Shader*>(shader)->_vkShader, FIRE_VK_FREE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Release(IPipeline* pipeline)
{
	vkDestroyPipeline(_device, static_cast<Vk_Pipeline*>(pipeline)->_vkPipeline, FIRE_VK_FREE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Release(IPipelineLayout* layout)
{
	vkDestroyPipelineLayout(_device, static_cast<Vk_PipelineLayout*>(layout)->_vkLayout, FIRE_VK_FREE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::Release(IRenderPass* renderPass)
{
	vkDestroyRenderPass(_device, static_cast<Vk_RenderPass*>(renderPass)->_vkRenderPass, FIRE_VK_FREE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t RenderSystem::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32 *memTypeFound)
{
	for (uint32_t i = 0; i < _memoryProperties.memoryTypeCount; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				if (memTypeFound)
				{
					*memTypeFound = true;
				}
				return i;
			}
		}
		typeBits >>= 1;
	}

	if (memTypeFound)
	{
		*memTypeFound = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Could not find a matching memory type");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::RegisterResourceMakers()
{
	_resourceMgr.AddResourceCache<IShader>();
	_resourceMgr.AddResourceCache<IShaderProgram>();
	_resourceMgr.AddResourceCache<IImage>();
	_resourceMgr.AddResourceCache<ITexture>();

	_resourceMgr.AddResourceMaker<IShader>([this]() -> shared_ptr<IShader>
	{
		return CreateShader();
	});
	_resourceMgr.AddResourceMaker<IShaderProgram>([this]() -> shared_ptr<IShaderProgram>
	{
		return CreateShaderProgram();
	});
	_resourceMgr.AddResourceMaker<ITexture>([this]() -> shared_ptr<ITexture>
	{
		return nullptr;
		//return CreateTexture();
	});
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

	FIRE_VK_VALIDATE(vkCreateInstance(&createInfo, nullptr, &_instance));
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

	FIRE_VK_VALIDATE(CreateDebugUtilsMessengerEXT(_instance, &debugCreateInfo, nullptr, &_callback));
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
	vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_memoryProperties);
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
	FIRE_VK_VALIDATE(vkCreateDevice(_physicalDevice, &deviceInfo, nullptr, &_device));

	vkGetDeviceQueue(_device, indices.GraphicsFamily.value(), 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, indices.PresentFamily.value(), 0, &_presentQueue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateSurface()
{
	FIRE_VK_VALIDATE(glfwCreateWindowSurface(_instance, _window.GetWindowHandle(), nullptr, &_surface));
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

	FIRE_VK_VALIDATE(vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain));

	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

	_swapChainImageFormat = surfaceFormat.format;
	_swapChainExtent = extent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateImageViews()
{
	size_t num = _swapChainImages.size();
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

		FIRE_VK_VALIDATE(vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderSystem::CreateFramebuffers()
{
	_swapChainFrameBuffers.resize(_swapChainImageViews.size());
	for(size_t i=0; i<_swapChainImageViews.size(); ++i)
	{
		VkImageView attachments[] ={
			_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		//framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = _swapChainExtent.width;
		framebufferInfo.height = _swapChainExtent.height;
		framebufferInfo.layers = 1;
	}
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