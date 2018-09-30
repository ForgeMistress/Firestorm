///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SHADERRESOURCE_H_
#define LIBSCENE_SHADERRESOURCE_H_
#pragma once

#include "RenderMgr.h"

#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

#include <LLGL/VertexFormat.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgramLoader final : public ResourceLoader
{
public:
	ShaderProgramLoader(RenderMgr& renderMgr);
	~ShaderProgramLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;
private:
	RenderMgr&                              _renderMgr;
	Json::CharReaderBuilder                 _builder;
	Json::CharReader*                       _reader;
	ObjectPool<class ShaderProgramResource> _shaderPool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderLoader final : public ResourceLoader
{
public:
	ShaderLoader(RenderMgr& renderMgr);
	~ShaderLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;
private:
	RenderMgr& _renderMgr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(ShaderResource, ShaderLoader);
public:
	ShaderResource(RenderMgr& renderMgr, LLGL::ShaderType shaderType);
	virtual ~ShaderResource();

	virtual bool IsReady() const;

	LLGL::Shader* GetShader() const;
private:
	String _shaderData;
	LLGL::ShaderType _shaderType;
	LLGL::Shader* _shader{ nullptr };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgramResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(ShaderProgramResource, ShaderProgramLoader);
public:
	ShaderProgramResource(RenderMgr& renderMgr);
	virtual ~ShaderProgramResource();

	virtual bool IsReady() const;

	LLGL::ShaderProgram* GetProgram() const;

	LLGL::ShaderProgram* Compile(std::initializer_list<LLGL::VertexFormat> vertexFormats);
private:
	void PurgeCompiledShaders();
	LLGL::Shader* MakeShader(LLGL::ShaderType shaderType);
	//bool AddShaderData(LLGL::ShaderType type, const String& data);
	void AddDependency(Resource&& resource);
	bool CompileShader(LLGL::ShaderType type);

	RenderMgr&                                    _renderMgr;
	// UnorderedMap<LLGL::ShaderType, LLGL::Shader*> _shaders;
	UnorderedMap<LLGL::ShaderType, Resource>      _shaders;
	// UnorderedMap<LLGL::ShaderType, String>        _shaderData;
	LLGL::ShaderProgram*                          _shaderProgram{ nullptr };
	bool                                          _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif