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

#include <libIO/ResourceReference.h>
#include <libIO/ResourceMgr.h>

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

private:
	RenderMgr& _renderMgr;
	Vector<LLGL::Shader*> _shaders;
	LLGL::ShaderProgram* _shaderProgram{ nullptr };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ShaderLoader final : public ResourceLoader
{
	ShaderLoader(RenderMgr& renderMgr);
	~ShaderLoader();

	virtual ResourceMgr::LoadResult Load(const ResourceReference& ref) override;
private:
	LLGL::Shader* MakeShader(const Vector<char>& data, LLGL::ShaderType shaderType);

	RenderMgr&                 _renderMgr;
	Json::CharReaderBuilder    _builder;
	Json::CharReader*          _reader;
	ObjectPool<ShaderResource> _shaderPool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <>
struct ResourceTraits<ShaderResource>
{
	using type   = ShaderResource;
	using loader = ShaderLoader;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif