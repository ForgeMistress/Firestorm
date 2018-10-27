///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IRenderPass
//
//  IRenderPass 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_IRENDERPASS_H_
#define LIBSCENE_IRENDERPASS_H_
#pragma once

#include "Enumerations.h"

OPEN_NAMESPACE(Firestorm);

class IRenderPass
{
public:
	struct CreateInfo
	{
		struct AttachmentDescription
		{
			AttachmentDescriptionFlags    Flags;
			size_t                        Format;
			SampleCountFlags              Samples{ SampleCountFlags::k1_BIT };
			AttachmentLoadOp              LoadOp{ AttachmentLoadOp::kCLEAR };
			AttachmentStoreOp             StoreOp{ AttachmentStoreOp::kSTORE };
			AttachmentLoadOp              StencilLoadOp{ AttachmentLoadOp::kDONT_CARE };
			AttachmentStoreOp             StencilStoreOp{ AttachmentStoreOp::kDONT_CARE };

			/// specifies which layout the image will have before the render pass begins
			ImageLayout InitialLayout{ ImageLayout::kUNDEFINED };

			/// specifies the layout to automatically transition to when the render pass finishes
			ImageLayout FinalLayout{ ImageLayout::kPRESENT_SRC };
		};
		vector<AttachmentDescription> Attachments;
		void AddAttachmentDesc(const AttachmentDescription& attachmentDesc);
		

		struct AttachmentReference
		{
			uint32_t    Attachment{ 9001 };
			ImageLayout Layout{ ImageLayout::kUNDEFINED };
		};

		struct SubpassDescription
		{

			//RenderFlags                     Flags;
			PipelineBindPoint               PipelineBindPoint{ PipelineBindPoint::kGRAPHICS };
			vector<AttachmentReference>     ColorAttachments;
			vector<AttachmentReference>     InputAttachments;
			vector<AttachmentReference>     ResolveAttachments;
			vector<AttachmentReference>     DepthStencilAttachments;
			vector<uint32_t>                PreserveAttachments;
		};
		vector<SubpassDescription> Subpasses;
		void AddSubpass(const SubpassDescription& subpass);

		CreateInfo(const class RenderSystem& renderSystem);

		const class RenderSystem& _renderSystem;
	};

	virtual ~IRenderPass() = default;
};

CLOSE_NAMESPACE(Firestorm);

#endif