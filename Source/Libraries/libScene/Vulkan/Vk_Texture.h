///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Vk_Texture
//
//  Implementation of ITexture for Vulkan.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKTEXTURE_H_
#define LIBSCENE_VKTEXTURE_H_
#pragma once

#include "../RenderMgr.h"
#include "../ITexture.h"

OPEN_NAMESPACE(Firestorm);

class Vk_Texture : public ITexture
{
	FIRE_RESOURCE_DECLARE(Vk_Texture, ITexture);
public:
	Vk_Texture(class RenderSystem& renderSystem);
	virtual ~Vk_Texture();

	bool IsReady() const override { return true; }
private:
	class RenderSystem& _renderSystem;
};

CLOSE_NAMESPACE(Firestorm);


#endif