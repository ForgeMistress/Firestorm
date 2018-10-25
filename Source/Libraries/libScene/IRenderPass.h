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
			VkFormat                      Format;
			SampleCountFlags              Samples;
			AttachmentLoadOp              LoadOp;
			AttachmentStoreOp             StoreOp;
			AttachmentLoadOp              StencilLoadOp;
			AttachmentStoreOp             StencilStoreOp;
			ImageLayout                   InitialLayout;
			ImageLayout                   FinalLayout;
		};
		

		struct AttachmentReference
		{
			uint32_t       Attachment;
			ImageLayout    Layout;
		};

		struct
		{

			//RenderFlags                     Flags;
			PipelineBindPoint               PipelineBindPoint;
			AttachmentDescription ColorAttachment;
			//vector<AttachmentReference>     InputAttachments;
			//vector<AttachmentReference>     ColorAttachments;
			//vector<AttachmentReference>     ResolveAttachments;
			//vector<AttachmentReference>     DepthStencilAttachments;
			//vector<uint32_t>                PreserveAttachments;
		} SubpassDescription;
		//vector<SubpassDescription> Subpasses;
	};

	virtual ~IRenderPass() = default;
};

CLOSE_NAMESPACE(Firestorm);

#endif