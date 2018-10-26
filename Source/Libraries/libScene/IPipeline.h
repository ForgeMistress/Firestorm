///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IPipeline
//
//  IPipeline 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_IPIPELINE_H_
#define LIBSCENE_IPIPELINE_H_
#pragma once

#include "ShaderProgram.h"
#include "PrimitiveTopology.h"
#include "Enumerations.h"
#include "IRenderPass.h"
#include "ShaderProgram.h"
#include <libMath/Vector.h>

OPEN_NAMESPACE(Firestorm);

class IPipelineLayout
{
public:
	struct CreateInfo
	{
		//const void*                     Next;
		//VkPipelineLayoutCreateFlags     Flags;
		//uint32_t                        SetLayoutCount;
		//const VkDescriptorSetLayout*    SetLayouts;
		//uint32_t                        PushConstantRangeCount;
		//const VkPushConstantRange*      PushConstantRanges;
	};

	virtual ~IPipelineLayout() = default;
};

class IPipeline
{
public:
	struct CreateInfo
	{
		struct VertexInputState
		{
		};
		VertexInputState VertexInputState;

		struct InputAssembly
		{
			//const void*                                pNext;
			//VkPipelineInputAssemblyStateCreateFlags    flags;
			PrimitiveTopology Topology{ PrimitiveTopology::kTRIANGLE_STRIP };
			bool PrimitiveRestartEnable{ false };
		};
		InputAssembly InputAssembly;

		struct Viewport
		{
			Viewport(float posX, float posY, float extentX, float extentY, float minDepth, float maxDepth)
				: PosX(posX)
				, PosY(posY)
				, ExtentX(extentX)
				, ExtentY(extentY)
				, MinDepth(minDepth)
				, MaxDepth(maxDepth) {}
			
			float PosX{ 0 }, PosY{ 0 };
			float ExtentX{ 0 }, ExtentY{ 0 };
			float MinDepth;
			float MaxDepth;
		};
		vector<Viewport> Viewports;

		struct Scissor
		{
			int32_t  OffsetX{ 0 }, OffsetY{ 0 };
			uint32_t ExtentX{ 1 }, ExtentY{ 1 };
		};
		vector<Scissor> Scissors;

		struct RasterizerState
		{
			bool        DepthClampEnable{ false };
			bool        RasterizerDiscardEnable{ false };
			PolyMode    PolygonMode{ PolyMode::kFill };
			float       LineWidth{ 1.0f };
			CullingMode CullMode{ CullingMode::kBACK_BIT };
			FrontFace   FrontFace{ FrontFace::kCLOCKWISE };
			bool        DepthBiasEnable{ false };
		};
		RasterizerState RasterizerState;

		struct MultisampleState
		{
			bool SampleShadingEnable{ false };
			SampleCountFlags RasterSamples{ SampleCountFlags::k1_BIT };
		};
		MultisampleState MultisampleState;

		struct ColorBlendAttachmentState
		{
			bool                BlendEnable{ false };
			BlendFactor         SrcColorBlendFactor;
			BlendFactor         DstColorBlendFactor;
			BlendOp             ColorBlendOp;
			BlendFactor         SrcAlphaBlendFactor;
			BlendFactor         DstAlphaBlendFactor;
			BlendOp             AlphaBlendOp;
			ColorComponentFlags ColorWriteMask{
				ColorComponentFlags::kR_BIT |
				ColorComponentFlags::kG_BIT |
				ColorComponentFlags::kB_BIT |
				ColorComponentFlags::kA_BIT
			};
		};

		struct PipelineColorBlendState
		{
			const void*                          Next{ nullptr };
			//VkPipelineColorBlendStateCreateFlags Flags;
			bool                                 LogicOpEnable{ false };
			LogicOp                              LogicOp;
			vector<ColorBlendAttachmentState>    Attachments;
			float                                BlendConstants[4];
		};
		PipelineColorBlendState PipelineColorBlendState;

		CreateInfo(class RenderSystem& renderSystem);

		Resource<IShaderProgram>  ShaderStage;
		IPipelineLayout*          Layout{ nullptr };
		IRenderPass*              RenderPass{ nullptr };
	};

	virtual ~IPipeline() = default;
};

CLOSE_NAMESPACE(Firestorm);

#endif