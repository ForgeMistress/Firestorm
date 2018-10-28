///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKSHADERPROGRAMRESOURCE_H_
#define LIBSCENE_VKSHADERPROGRAMRESOURCE_H_
#pragma once

#include "VulkanPlatform.h"
#include "../Enumerations.h"
#include "../IShader.h"

#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/ResourceReference.h>
#include <libIO/ResourceHandle.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Vk_Shader final : public IShader
{
	FIRE_RESOURCE_DECLARE(Vk_Shader, IShader);
public:
	Vk_Shader(class RenderSystem& system);
	virtual ~Vk_Shader();

	virtual bool IsReady() const;

	virtual ShaderType GetType() const override { return _type; }

	VkShaderModule GetModule() const { return _vkShader; }
private:
	friend class RenderSystem;
	friend class Vk_ShaderProgram;

	class RenderSystem& _renderSystem;
	ShaderType          _type{ ShaderType::kVERTEX };
	VkShaderModule      _vkShader;
	bool                _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Vk_ShaderProgram final : public IShaderProgram
{
	FIRE_RESOURCE_DECLARE(Vk_ShaderProgram, IShaderProgram);
public:
	Vk_ShaderProgram(class RenderSystem& system);
	virtual ~Vk_ShaderProgram();

	virtual bool IsReady() const;
private:
	friend class RenderSystem;

	class RenderSystem&       _renderSystem;
	std::mutex                _slock;
	vector<Resource<IShader>> _shaders;
	bool                      _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif