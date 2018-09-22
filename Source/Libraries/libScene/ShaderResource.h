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

#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

#include "RenderMgr.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderResource final : public IResourceObject
{
	FIRE_MIRROR_DECLARE(ShaderResource);
	friend struct ShaderLoader;
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

struct ShaderLoader final : public ResourceLoader
{
	ShaderLoader(RenderMgr& renderMgr);
	~ShaderLoader();

	virtual LoadResult Load(const ResourceReference& ref) override;
private:
	LLGL::Shader* MakeShader(const Vector<char>& data, LLGL::ShaderType shaderType);

	RenderMgr&                 _renderMgr;
	Json::CharReaderBuilder    _builder;
	Json::CharReader*          _reader;
	ObjectPool<ShaderResource> _shaderPool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif