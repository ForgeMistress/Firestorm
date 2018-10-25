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

#include "../RenderMgr.h"

#include <libApp/Application.h>
#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>
#include <libIO/ResourceHandle.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum struct ShaderType
{
	kVERTEX = 0x00000001,
	kTESSELLATION_CONTROL = 0x00000002,
	kTESSELLATION_EVALUATION = 0x00000004,
	kGEOMETRY = 0x00000008,
	kFRAGMENT = 0x00000010,
	kCOMPUTE = 0x00000020,
	kALL_GRAPHICS = 0x0000001F,
};
class Shader final : public IResourceObject
{
	FIRE_RESOURCE_DECLARE(Shader);
public:
	Shader(Application& app, ShaderType type);
	virtual ~Shader();

	virtual bool IsReady() const;

	ShaderType GetType() const { return _type; }
	VkShaderModule GetModule() const { return _shader; }
private:
	friend class RenderSystem;
	friend class ShaderProgram;

	RenderSystem&  _renderSystem;
	ShaderType     _type{ ShaderType::kVERTEX };
	VkShaderModule _shader;
	bool           _isCompiled{ false };
};

class ShaderProgram final : public IResourceObject
{
	FIRE_RESOURCE_DECLARE(ShaderProgram);
public:
	ShaderProgram(Application& app);
	virtual ~ShaderProgram();

	virtual bool IsReady() const;
	void Compile();
private:
	friend class RenderSystem;
	void AddShader(Resource<Shader>& shader);

	class RenderMgr&         _renderMgr;
	std::mutex               _slock;
	vector<Resource<Shader>> _shaders;
	bool                     _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif