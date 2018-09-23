///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderResource
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

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderLoader final : public ResourceLoader
{
public:
	ShaderLoader(RenderMgr& renderMgr);
	~ShaderLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;
private:
	LLGL::Shader* MakeShader(const Vector<char>& data, LLGL::ShaderType shaderType);

	RenderMgr&                 _renderMgr;
	Json::CharReaderBuilder    _builder;
	Json::CharReader*          _reader;
	ObjectPool<class ShaderResource> _shaderPool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(ShaderResource, ShaderLoader);
public:
	ShaderResource(RenderMgr& renderMgr);
	virtual ~ShaderResource();

	LLGL::ShaderProgram* GetProgram() const;

	bool Compile();
private:
	void PurgeCompiledShaders();
	RenderMgr&                                    _renderMgr;
	UnorderedMap<LLGL::ShaderType, LLGL::Shader*> _shaders;
	UnorderedMap<LLGL::ShaderType, Vector<char>>  _shaderData;
	LLGL::ShaderProgram*                          _shaderProgram{ nullptr };
	bool                                          _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif